#include "WOTLK-SDK/Discord.h"

#include <windows.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <format>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#if defined(WOTLK_SDK_HAS_DISCORD_GAME_SDK)
#if __has_include(<discord-game-sdk/discord.h>)
#include <discord-game-sdk/discord.h>
#define WOTLK_SDK_DISCORD_HEADER_FOUND 1
#elif __has_include(<discord.h>)
#include <discord.h>
#define WOTLK_SDK_DISCORD_HEADER_FOUND 1
#endif
#endif

namespace WotlkSDK {
namespace Discord {

namespace {

/**
 * @brief Wire protocol header for Discord IPC Named Pipes.
 *
 * Each frame transferred across \\.\pipe\discord-ipc-N starts with an 8-byte header:
 * - Opcode (4 bytes, little-endian): 0=Handshake, 1=Frame, 2=Close, 3=Ping, 4=Pong
 * - Length (4 bytes, little-endian): byte length of the ensuing UTF-8 JSON payload
 */
#pragma pack(push, 1)
struct DiscordHeader {
    uint32_t opcode;  /**< Packet opcode identifier. */
    uint32_t length;  /**< Payload length in bytes. */
};
#pragma pack(pop)

// Thread synchronization and lifecycle state
std::atomic<bool> g_running{false};
std::thread g_workerThread;
std::mutex g_mutex;
std::condition_variable g_cv;

// IPC pipe state
HANDLE g_pipeHandle = INVALID_HANDLE_VALUE;
std::string g_clientId;
uint32_t g_intervalMs = 5000;
int64_t g_startTime = 0;
uint64_t g_nonce = 0;

// Runtime activity formatting callback
ActivityCallback g_customCallback = nullptr;

#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
// Discord Social SDK (Game SDK) state
discord::Core* g_discordCore = nullptr;
bool g_usingGameSdk = false;
#endif

/**
 * @brief Escapes characters in strings for safe JSON serialization.
 * @param s Input string view.
 * @return Escaped JSON-compliant string.
 */
std::string escapeJson(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:   out += c; break;
        }
    }
    return out;
}

/**
 * @brief Writes an opcode header and payload to the connected named pipe.
 * @param pipe Target named pipe handle.
 * @param opcode Discord IPC packet opcode.
 * @param payload Raw JSON payload string.
 * @return true if the full packet was written successfully, false otherwise.
 */
bool sendPacket(HANDLE pipe, uint32_t opcode, std::string_view payload) {
    if (pipe == INVALID_HANDLE_VALUE) { return false; }

    DiscordHeader header{opcode, static_cast<uint32_t>(payload.size())};

    DWORD written = 0;
    if (!WriteFile(pipe, &header, sizeof(header), &written, nullptr) || written != sizeof(header)) {
        return false;
    }

    if (!payload.empty()) {
        if (!WriteFile(pipe, payload.data(), static_cast<DWORD>(payload.size()), &written, nullptr) ||
            written != payload.size()) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Reads a single packet header and payload from the named pipe.
 * @param pipe Target named pipe handle.
 * @param out_opcode Output variable for the received opcode.
 * @param out_payload Output buffer for the received payload.
 * @return true if packet was successfully received, false on error or disconnection.
 */
bool readPacket(HANDLE pipe, uint32_t& out_opcode, std::string& out_payload) {
    if (pipe == INVALID_HANDLE_VALUE) { return false; }

    DiscordHeader header{};
    DWORD read = 0;
    if (!ReadFile(pipe, &header, sizeof(header), &read, nullptr) || read != sizeof(header)) {
        return false;
    }

    out_opcode = header.opcode;
    out_payload.resize(header.length);

    if (header.length > 0) {
        if (!ReadFile(pipe, out_payload.data(), header.length, &read, nullptr) || read != header.length) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Iterates over possible Discord named pipes (0 to 9) and attempts connection.
 * @return Valid pipe handle if connected, INVALID_HANDLE_VALUE if Discord is not running.
 */
HANDLE connectPipe() {
    char pipeName[64];
    for (int i = 0; i < 10; ++i) {
        std::snprintf(pipeName, sizeof(pipeName), "\\\\.\\pipe\\discord-ipc-%d", i);
        HANDLE h = CreateFileA(pipeName, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            return h;
        }
    }
    return INVALID_HANDLE_VALUE;
}

/**
 * @brief Sends the Discord close frame and closes the named pipe handle.
 */
void disconnectPipe() {
    if (g_pipeHandle != INVALID_HANDLE_VALUE) {
        sendPacket(g_pipeHandle, 2, "{}");
        CloseHandle(g_pipeHandle);
        g_pipeHandle = INVALID_HANDLE_VALUE;
    }
}

/**
 * @brief Captures live game state, formats the activity payload, and sends SET_ACTIVITY over IPC.
 * @param pipe Active named pipe handle.
 */
void sendActivityPayload(HANDLE pipe) {
    auto presenceData = PRESENCE::GET_DATA();

    Activity activity{};
    {
        std::lock_guard lock(g_mutex);
        if (g_customCallback) {
            g_customCallback(presenceData, activity);
        } else {
            DefaultFormatter::Format(presenceData, activity);
        }
    }

    int64_t start = activity.start_timestamp > 0 ? activity.start_timestamp : g_startTime;

    std::string assetsPart;
    if (!activity.large_image_key.empty() || !activity.small_image_key.empty()) {
        assetsPart = std::format(
            R"(,"assets":{{"large_image":"{}","large_text":"{}","small_image":"{}","small_text":"{}"}})",
            escapeJson(activity.large_image_key),
            escapeJson(activity.large_image_text),
            escapeJson(activity.small_image_key),
            escapeJson(activity.small_image_text)
        );
    }

    std::string partyPart;
    if (activity.party_size > 0 && activity.party_max >= activity.party_size) {
        partyPart = std::format(R"(,"party":{{"size":[{},{}]}})", activity.party_size, activity.party_max);
    }

    std::string payload = std::format(
        R"({{"cmd":"SET_ACTIVITY","args":{{"pid":{},"activity":{{"state":"{}","details":"{}","timestamps":{{"start":{}}}{}{}}}}},"nonce":"{}"}})",
        GetCurrentProcessId(),
        escapeJson(activity.state),
        escapeJson(activity.details),
        start,
        assetsPart,
        partyPart,
        ++g_nonce
    );

    sendPacket(pipe, 1, payload);

    // Drain and discard Discord client response frame
    uint32_t respOp = 0;
    std::string respPayload;
    readPacket(pipe, respOp, respPayload);
}

#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
/**
 * @brief Dispatches presence updates through the official Discord Social SDK (Game SDK) ActivityManager.
 */
void sendSocialSdkActivity() {
    if (!g_discordCore) { return; }

    auto presenceData = PRESENCE::GET_DATA();

    Activity activity{};
    {
        std::lock_guard lock(g_mutex);
        if (g_customCallback) {
            g_customCallback(presenceData, activity);
        } else {
            DefaultFormatter::Format(presenceData, activity);
        }
    }

    discord::Activity act{};
    act.SetType(discord::ActivityType::Playing);
    act.SetState(activity.state.c_str());
    act.SetDetails(activity.details.c_str());

    int64_t start = activity.start_timestamp > 0 ? activity.start_timestamp : g_startTime;
    act.GetTimestamps().SetStart(start);

    if (!activity.large_image_key.empty()) {
        act.GetAssets().SetLargeImage(activity.large_image_key.c_str());
        act.GetAssets().SetLargeText(activity.large_image_text.c_str());
    }
    if (!activity.small_image_key.empty()) {
        act.GetAssets().SetSmallImage(activity.small_image_key.c_str());
        act.GetAssets().SetSmallText(activity.small_image_text.c_str());
    }
    if (activity.party_size > 0 && activity.party_max >= activity.party_size) {
        act.GetParty().GetSize().SetCurrentSize(activity.party_size);
        act.GetParty().GetSize().SetMaxSize(activity.party_max);
    }

    g_discordCore->ActivityManager().UpdateActivity(act, [](discord::Result /*result*/) {});
}
#endif

/**
 * @brief Background worker loop responsible for connection maintenance and periodic status updates.
 */
void workerLoop() {
#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    if (g_usingGameSdk && g_discordCore) {
        sendSocialSdkActivity();

        while (g_running.load()) {
            if (g_discordCore) {
                g_discordCore->RunCallbacks();
                sendSocialSdkActivity();
            }

            std::unique_lock lock(g_mutex);
            if (g_cv.wait_for(lock, std::chrono::milliseconds(g_intervalMs), []() {
                return !g_running.load();
            })) {
                break;
            }
        }

        if (g_discordCore) {
            delete g_discordCore;
            g_discordCore = nullptr;
        }
        g_usingGameSdk = false;
        return;
    }
#endif

    while (g_running.load()) {
        // Attempt connection if disconnected
        if (g_pipeHandle == INVALID_HANDLE_VALUE) {
            g_pipeHandle = connectPipe();
            if (g_pipeHandle != INVALID_HANDLE_VALUE) {
                // Perform initial handshake (Opcode 0)
                std::string handshake = std::format(R"({{"v":1,"client_id":"{}"}})", g_clientId);
                if (sendPacket(g_pipeHandle, 0, handshake)) {
                    uint32_t op = 0;
                    std::string resp;
                    readPacket(g_pipeHandle, op, resp);
                } else {
                    disconnectPipe();
                }
            }
        }

        // Dispatch presence update if pipe is open
        if (g_pipeHandle != INVALID_HANDLE_VALUE) {
            sendActivityPayload(g_pipeHandle);
        }

        // Suspend until next cadence tick or shutdown notification
        std::unique_lock lock(g_mutex);
        g_cv.wait_for(lock, std::chrono::milliseconds(g_intervalMs), []() {
            return !g_running.load();
        });
    }

    disconnectPipe();
}

}  // namespace

bool Start(std::string_view clientId, uint32_t intervalMs) {
    if (clientId.empty()) { return false; }
    if (g_running.load()) { return true; }

    g_clientId = clientId;
    g_intervalMs = intervalMs > 500 ? intervalMs : 5000;
    g_startTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    int64_t idNum = 0;
    try {
        idNum = std::stoll(std::string(clientId));
    } catch (...) {
        idNum = 0;
    }

    if (idNum != 0) {
        discord::Core* core = nullptr;
        auto res = discord::Core::Create(idNum, DiscordCreateFlags_NoRequireDiscord, &core);
        if (res == discord::Result::Ok && core) {
            g_discordCore = core;
            g_usingGameSdk = true;
        }
    }
#endif

    g_running.store(true);
    g_workerThread = std::thread(workerLoop);
    return true;
}

void Stop() {
    if (!g_running.load()) { return; }

    g_running.store(false);
    g_cv.notify_all();

    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }

    disconnectPipe();

#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    if (g_discordCore) {
        delete g_discordCore;
        g_discordCore = nullptr;
    }
    g_usingGameSdk = false;
#endif
}

void Update() {
    if (!g_running.load()) { return; }

#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    if (g_usingGameSdk && g_discordCore) {
        sendSocialSdkActivity();
        return;
    }
#endif

    if (g_pipeHandle != INVALID_HANDLE_VALUE) {
        sendActivityPayload(g_pipeHandle);
    }
}

bool IsConnected() {
#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    if (g_running.load() && g_usingGameSdk && g_discordCore != nullptr) {
        return true;
    }
#endif
    return g_running.load() && g_pipeHandle != INVALID_HANDLE_VALUE;
}

void SetCallback(ActivityCallback callback) {
    std::lock_guard lock(g_mutex);
    g_customCallback = std::move(callback);
}

void* GetCore() {
#if defined(WOTLK_SDK_DISCORD_HEADER_FOUND)
    return static_cast<void*>(g_discordCore);
#else
    return nullptr;
#endif
}

}  // namespace Discord
}  // namespace WotlkSDK
