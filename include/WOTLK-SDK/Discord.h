#pragma once

/**
 * @file Discord.h
 * @brief Threaded Discord Social Rich Presence subsystem for World of Warcraft 3.3.5a (build 12340).
 *
 * Implements a standalone, zero-dependency Discord Rich Presence IPC client communicating
 * directly with the local Discord Desktop client via Windows Named Pipes (\\.\pipe\discord-ipc-0..9).
 * Eliminates external runtime DLL dependencies such as discord-rpc.dll or discord_game_sdk.dll.
 */

#include <cstdint>
#include <string>
#include <string_view>
#include <functional>
#include <memory>
#include <format>
#include <cctype>

#include "Defines.h"
#include "API.h"

#ifndef DISCORD_CLIENT_ID
/**
 * @def DISCORD_CLIENT_ID
 * @brief Default empty definition for Discord Application Client ID.
 *
 * Override by defining DISCORD_CLIENT_ID prior to including SDK headers or passing
 * -DDISCORD_CLIENT_ID="<id>" via compiler flags.
 */
#define DISCORD_CLIENT_ID ""
#endif

namespace WotlkSDK {
namespace Discord {

/**
 * @brief Represents a Discord Rich Presence Activity payload.
 *
 * Conforms to the Discord RPC SET_ACTIVITY schema.
 */
struct Activity {
    std::string state;               /**< Secondary activity description (e.g., "Dungeon Queue", "Raid Queue", "Dalaran"). */
    std::string details;             /**< Primary activity status (e.g., "Level 80 Human Paladin"). */
    std::string large_image_key;     /**< Asset key for the large image uploaded to the Discord Developer Portal. */
    std::string large_image_text;    /**< Tooltip hover text for the large image asset. */
    std::string small_image_key;     /**< Asset key for the small overlay image (e.g., class or faction badge). */
    std::string small_image_text;    /**< Tooltip hover text for the small overlay image. */
    int64_t     start_timestamp = 0; /**< Unix epoch timestamp in seconds indicating activity start. Set to 0 for SDK auto-management. */
    int64_t     end_timestamp = 0;   /**< Optional Unix epoch timestamp in seconds indicating queue/activity completion or ETA. */
    int         party_size = 0;      /**< Current party or raid member count (e.g., 1, 4). Set to 0 to omit. */
    int         party_max = 0;       /**< Maximum party or raid capacity (e.g., 4, 8, 5, 25). Set to 0 to omit. */
    std::string party_id;            /**< Unique party or lobby ID string. */
    uint32_t    supported_platforms = 1; /**< Bitfield flags for supported platforms (1=Desktop, 2=Android, 4=iOS). */
};

/**
 * @brief Standard activity formatter mapping World of Warcraft client state to Discord presence.
 *
 * Extracts progression, zone, class, queue, and party snapshots from PRESENCE::GET_DATA()
 * to generate informative Discord rich presence statuses.
 */
struct DefaultFormatter {
    /**
     * @brief Formats raw game presence snapshot into a Discord Activity payload.
     * @param in Live game state snapshot captured from client engine memory.
     * @param out Destination Activity structure to populate.
     */
    static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, Activity& out) {
        out.supported_platforms = in.supported_platforms > 0 ? in.supported_platforms : 1;
        out.start_timestamp = in.start_timestamp;
        out.end_timestamp = in.end_timestamp;

        if (in.is_in_world) {
            if (in.character_name && in.character_name[0] != '\0') {
                out.details = std::format("{} - Lvl {} {} {}", in.character_name, in.level, in.race_name, in.class_name);
            } else {
                out.details = std::format("Level {} {} {}", in.level, in.race_name, in.class_name);
            }

            // State priority: Active queue state (e.g., "Dungeon Queue", "Raid Queue") -> zone / subzone
            if (in.is_in_queue && in.queue_state && in.queue_state[0] != '\0') {
                out.state = in.queue_state;
            } else if (in.subzone_text && in.subzone_text[0] != '\0') {
                out.state = std::format("{} - {}", in.zone_text, in.subzone_text);
            } else if (in.zone_text && in.zone_text[0] != '\0') {
                out.state = in.zone_text;
            } else {
                out.state = in.continent_name && in.continent_name[0] != '\0' ? in.continent_name : "In World";
            }

            // Party representation (e.g. 1/4, 4/8, 5/5, 10/10, 25/25)
            if (in.party_size > 0 && in.party_max >= in.party_size) {
                out.party_size = static_cast<int>(in.party_size);
                out.party_max = static_cast<int>(in.party_max);
                out.party_id = "wotlk_party";
            } else if (in.is_in_group) {
                out.party_size = in.party_size > 0 ? static_cast<int>(in.party_size) : 1;
                out.party_max = in.party_max > 0 ? static_cast<int>(in.party_max) : (in.is_in_raid ? 25 : 5);
                out.party_id = "wotlk_party";
            }

            out.large_image_key = "wotlk_logo";
            out.large_image_text = (in.zone_text && in.zone_text[0] != '\0') ? in.zone_text : "World of Warcraft";

            std::string class_key = in.class_name ? in.class_name : "";
            for (auto& c : class_key) { c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
            out.small_image_key = class_key;
            out.small_image_text = in.class_name ? in.class_name : "";
        } else if (in.game_state == 1) {
            out.details = "Character Selection";
            if (in.character_name && in.character_name[0] != '\0') {
                out.state = std::format("Selected: {} (Lvl {} {})", in.character_name, in.level, in.class_name);
            } else {
                out.state = "Choosing Character...";
            }
            out.large_image_key = "wotlk_logo";
            out.large_image_text = "Wrath of the Lich King";
            out.small_image_key = "";
            out.small_image_text = "";
        } else {
            out.details = (in.game_state_name && in.game_state_name[0] != '\0') ? in.game_state_name : "Main Menu";
            out.state = in.is_loading ? "Loading..." : "Connecting to Realm...";
            out.large_image_key = "wotlk_logo";
            out.large_image_text = "Wrath of the Lich King";
            out.small_image_key = "";
            out.small_image_text = "";
        }
    }
};

/**
 * @brief Function signature for custom runtime activity formatting callbacks.
 */
using ActivityCallback = std::function<void(const PRESENCE::RICH_PRESENCE_DATA&, Activity&)>;

/**
 * @brief Starts the background Discord Rich Presence worker thread.
 *
 * Automatically connects to the local Discord desktop client through named pipes,
 * performs the handshake, and dispatches periodic activity updates.
 *
 * @param clientId Discord Application Client ID registered in the Discord Developer Portal.
 * @param intervalMs Refresh cadence in milliseconds between periodic status updates. Default is 5000 ms.
 * @return true if the worker thread started successfully, false if the client ID was invalid.
 */
WOTLK_SDK_API bool Start(std::string_view clientId = DISCORD_CLIENT_ID, uint32_t intervalMs = 5000);

/**
 * @brief Stops the background Discord Rich Presence worker thread and disconnects the IPC pipe.
 */
WOTLK_SDK_API void Stop();

/**
 * @brief Triggers an immediate presence payload synchronization with the Discord desktop client.
 */
WOTLK_SDK_API void Update();

/**
 * @brief Checks whether the Discord Rich Presence service currently holds an active IPC connection.
 * @return true if connected to the Discord desktop pipe, false otherwise.
 */
WOTLK_SDK_API bool IsConnected();

/**
 * @brief Installs a custom callable to format Discord activity payloads at runtime.
 * @param callback Callable accepting a const PRESENCE::RICH_PRESENCE_DATA& and Activity&.
 */
WOTLK_SDK_API void SetCallback(ActivityCallback callback);

/**
 * @brief Retrieves the active Discord Social SDK (Game SDK) Core instance handle.
 *
 * When built with Discord Social SDK enabled (via vcpkg port discord-game-sdk),
 * this function returns a pointer to the active discord::Core instance.
 * Cast to discord::Core* to access advanced features:
 * - Voice and chat overlay management
 * - Multiplayer lobbies, networking, and matchmaking
 * - Friends list and user relationship queries
 * - Discord user authentication and profile data
 *
 * @return Pointer to discord::Core if Social SDK backend is active, or nullptr.
 */
WOTLK_SDK_API void* GetCore();

/**
 * @brief Installs a custom C++ formatter type at compile-time.
 *
 * The formatter type must provide a static Format method:
 * @code
 * struct CustomFormatter {
 *     static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, DISCORD::Activity& out);
 * };
 * DISCORD::SET_FORMATTER<CustomFormatter>();
 * @endcode
 *
 * @tparam Formatter Struct or class implementing static void Format(const PRESENCE::RICH_PRESENCE_DATA&, Activity&).
 */
template <typename Formatter>
inline void SetFormatter() {
    SetCallback([](const PRESENCE::RICH_PRESENCE_DATA& in, Activity& out) {
        Formatter::Format(in, out);
    });
}

#if defined(DISCORD_RICH_PRESENCE)
namespace Internal {
    /**
     * @brief Internal RAII helper for compile-time auto-initialization when DISCORD_RICH_PRESENCE is defined.
     */
    struct DiscordAutoInit {
        DiscordAutoInit() {
            if constexpr (!std::string_view(DISCORD_CLIENT_ID).empty()) {
                ::WotlkSDK::Discord::Start(DISCORD_CLIENT_ID);
            }
        }
        ~DiscordAutoInit() {
            ::WotlkSDK::Discord::Stop();
        }
    };
    inline DiscordAutoInit g_discordAutoInit;
}  // namespace Internal
#endif

}  // namespace Discord

/* ========================================================================= */
/* SCREAMING_SNAKE_CASE & UpperCamelCase Aliases                             */
/* ========================================================================= */

/**
 * @brief SCREAMING_SNAKE_CASE accessors for the Discord Rich Presence subsystem.
 */
namespace DISCORD {
    using Activity = ::WotlkSDK::Discord::Activity;
    using DefaultFormatter = ::WotlkSDK::Discord::DefaultFormatter;

    /**
     * @brief Starts the background Discord Rich Presence worker thread.
     * @param clientId Discord Application Client ID.
     * @param intervalMs Refresh interval in milliseconds.
     * @return true on success, false on invalid client ID.
     */
    inline bool START(std::string_view clientId = DISCORD_CLIENT_ID, uint32_t intervalMs = 5000) {
        return ::WotlkSDK::Discord::Start(clientId, intervalMs);
    }

    /**
     * @brief Stops the background Discord Rich Presence worker thread.
     */
    inline void STOP() {
        ::WotlkSDK::Discord::Stop();
    }

    /**
     * @brief Forces an immediate activity update over the IPC pipe.
     */
    inline void UPDATE() {
        ::WotlkSDK::Discord::Update();
    }

    /**
     * @brief Checks if the Discord Rich Presence service is currently connected.
     * @return true if connected, false otherwise.
     */
    inline bool IS_CONNECTED() {
        return ::WotlkSDK::Discord::IsConnected();
    }

    /**
     * @brief Installs a custom activity formatter callback.
     * @param callback Formatting functor.
     */
    inline void SET_CALLBACK(::WotlkSDK::Discord::ActivityCallback callback) {
        ::WotlkSDK::Discord::SetCallback(std::move(callback));
    }

    /**
     * @brief Retrieves the active Discord Social SDK (Game SDK) Core instance handle.
     * @return Pointer to discord::Core if active, or nullptr.
     */
    inline void* GET_CORE() {
        return ::WotlkSDK::Discord::GetCore();
    }

    /**
     * @brief Installs a custom C++ template formatter struct or class.
     * @tparam Formatter Formatter type providing static void Format(const PRESENCE::RICH_PRESENCE_DATA&, Activity&).
     */
    template <typename Formatter>
    inline void SET_FORMATTER() {
        ::WotlkSDK::Discord::SetFormatter<Formatter>();
    }

    /**
     * @brief Sets the activity state text (e.g., "Dungeon Queue", "Raid Queue", "In Dungeon").
     * @param state State description.
     */
    inline void SET_STATE(std::string_view state) {
        ::PRESENCE::SET_QUEUE_STATE(std::string(state).c_str());
    }

    /**
     * @brief Sets the party/group size and capacity (e.g. 1/4, 4/8, 5/5, 10/10, 25/25).
     * @param size Current member count.
     * @param max Maximum capacity.
     */
    inline void SET_PARTY(uint32_t size, uint32_t max) {
        ::PRESENCE::SET_PARTY(size, max);
    }

    /**
     * @brief Sets activity timestamps (start timestamp and optional end/countdown ETA timestamp).
     * @param start Activity start Unix epoch timestamp in seconds.
     * @param end Optional activity completion/ETA Unix epoch timestamp in seconds.
     */
    inline void SET_TIMESTAMPS(int64_t start, int64_t end = 0) {
        ::PRESENCE::SET_TIMESTAMPS(start, end);
    }

    /**
     * @brief Sets supported platform flags for the activity (1=Desktop, 2=Android, 4=iOS).
     * @param platforms Platform bitmask.
     */
    inline void SET_SUPPORTED_PLATFORMS(uint32_t platforms) {
        ::PRESENCE::SET_SUPPORTED_PLATFORMS(platforms);
    }

    /**
     * @brief Helper to configure queue state, party members, and optional queue duration/eta.
     * @param queueName Queue name (e.g. "Dungeon Queue", "Raid Queue").
     * @param partySize Current party count (e.g. 1, 4).
     * @param partyMax Maximum party capacity (e.g. 4, 8, 5, 25).
     * @param estEndTimestamp Optional estimated queue finish timestamp.
     */
    inline void SET_QUEUE(std::string_view queueName, uint32_t partySize = 0, uint32_t partyMax = 0, int64_t estEndTimestamp = 0) {
        ::PRESENCE::SET_QUEUE(std::string(queueName).c_str(), partySize, partyMax, estEndTimestamp);
    }
}  // namespace DISCORD

}  // namespace WotlkSDK

namespace DISCORD = ::WotlkSDK::DISCORD;
