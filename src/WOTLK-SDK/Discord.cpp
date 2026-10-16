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

namespace WotlkSDK {
namespace Discord {

namespace {

#pragma pack(push, 1)
struct DiscordHeader {
    uint32_t opcode;  // 0 = Handshake, 1 = Frame, 2 = Close, 3 = Ping, 4 = Pong
    uint32_t length;  // Length of JSON payload
};
#pragma pack(pop)

std::atomic<bool> g_running{false};
std::thread g_workerThread;
std::mutex g_mutex;
std::condition_variable g_cv;

HANDLE g_pipeHandle = INVALID_HANDLE_VALUE;
std::string g_clientId;
uint32_t g_intervalMs = 5000;
int64_t g_startTime = 0;
uint64_t g_nonce = 0;

ActivityCallback g_customCallback = nullptr;

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

void disconnectPipe() {
    if (g_pipeHandle != INVALID_HANDLE_VALUE) {
        sendPacket(g_pipeHandle, 2, "{}"); // Close frame
        CloseHandle(g_pipeHandle);
        g_pipeHandle = INVALID_HANDLE_VALUE;
    }
}

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

    // Read and drain response
    uint32_t respOp = 0;
    std::string respPayload;
    readPacket(pipe, respOp, respPayload);
}

void workerLoop() {
    while (g_running.load()) {
        // 1. Connect if not connected
        if (g_pipeHandle == INVALID_HANDLE_VALUE) {
            g_pipeHandle = connectPipe();
            if (g_pipeHandle != INVALID_HANDLE_VALUE) {
                // Handshake (Opcode 0)
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

        // 2. Send activity update
        if (g_pipeHandle != INVALID_HANDLE_VALUE) {
            sendActivityPayload(g_pipeHandle);
        }

        // 3. Wait for interval or wake on shutdown
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
}

void Update() {
    if (g_running.load() && g_pipeHandle != INVALID_HANDLE_VALUE) {
        sendActivityPayload(g_pipeHandle);
    }
}

bool IsConnected() {
    return g_running.load() && g_pipeHandle != INVALID_HANDLE_VALUE;
}

void SetCallback(ActivityCallback callback) {
    std::lock_guard lock(g_mutex);
    g_customCallback = std::move(callback);
}

}  // namespace Discord
}  // namespace WotlkSDK
