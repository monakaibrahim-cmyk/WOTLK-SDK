#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <functional>
#include <memory>
#include <format>
#include <cctype>

#include "Defines.h"
#include "API.h"

// Default empty definition for DISCORD_CLIENT_ID
#ifndef DISCORD_CLIENT_ID
#define DISCORD_CLIENT_ID ""
#endif

namespace WotlkSDK {
namespace Discord {

/**
 * @brief Represents a Discord Rich Presence Activity payload.
 */
struct Activity {
    std::string state;               // What the player is doing (e.g. "Dalaran - Runeweaver Square")
    std::string details;             // Player status & progression (e.g. "Level 80 Human Paladin")
    std::string large_image_key;     // Key for the large image uploaded to Discord Developer Portal
    std::string large_image_text;    // Tooltip text for the large image
    std::string small_image_key;     // Key for the small image
    std::string small_image_text;    // Tooltip text for the small image
    int64_t     start_timestamp = 0; // Unix start timestamp (0 = auto-managed from SDK start)
    int         party_size = 0;      // Current party size (0 to omit)
    int         party_max = 0;       // Max party size (0 to omit)
};

/**
 * @brief Default Activity Formatter for World of Warcraft 3.3.5a.
 *
 * Developers can customize or override this via templates:
 *   DISCORD::SET_FORMATTER<MyCustomFormatter>();
 */
struct DefaultFormatter {
    static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, Activity& out) {
        if (in.is_in_world) {
            // Details: Character info
            if (in.character_name && in.character_name[0] != '\0') {
                out.details = std::format("{} - Lvl {} {} {}", in.character_name, in.level, in.race_name, in.class_name);
            } else {
                out.details = std::format("Level {} {} {}", in.level, in.race_name, in.class_name);
            }

            // State: Current zone & subzone
            if (in.subzone_text && in.subzone_text[0] != '\0') {
                out.state = std::format("{} - {}", in.zone_text, in.subzone_text);
            } else if (in.zone_text && in.zone_text[0] != '\0') {
                out.state = in.zone_text;
            } else {
                out.state = in.continent_name && in.continent_name[0] != '\0' ? in.continent_name : "In World";
            }

            // Assets:
            out.large_image_key = "wotlk_logo";
            out.large_image_text = (in.zone_text && in.zone_text[0] != '\0') ? in.zone_text : "World of Warcraft";

            // Small icon: Class icon key
            std::string class_key = in.class_name ? in.class_name : "";
            for (auto& c : class_key) { c = static_cast<char>(std::tolower(static_cast<unsigned char>(c))); }
            out.small_image_key = class_key;
            out.small_image_text = in.class_name ? in.class_name : "";
        } else if (in.game_state == 1) { // Character Selection
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

using ActivityCallback = std::function<void(const PRESENCE::RICH_PRESENCE_DATA&, Activity&)>;

// Core Discord Rich Presence Service API
WOTLK_SDK_API bool Start(std::string_view clientId = DISCORD_CLIENT_ID, uint32_t intervalMs = 5000);
WOTLK_SDK_API void Stop();
WOTLK_SDK_API void Update();
WOTLK_SDK_API bool IsConnected();
WOTLK_SDK_API void SetCallback(ActivityCallback callback);

/**
 * @brief Template override: install any custom C++ formatter class or struct.
 *
 * Example:
 *   struct MyFormatter {
 *       static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, Discord::Activity& out) {
 *           out.details = "Raiding Icecrown Citadel";
 *           out.state = in.zone_text;
 *           out.large_image_key = "custom_icc_banner";
 *       }
 *   };
 *   DISCORD::SET_FORMATTER<MyFormatter>();
 */
template <typename Formatter>
inline void SetFormatter() {
    SetCallback([](const PRESENCE::RICH_PRESENCE_DATA& in, Activity& out) {
        Formatter::Format(in, out);
    });
}

#if defined(DISCORD_RICH_PRESENCE)
namespace Internal {
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
namespace DISCORD {
    using Activity = ::WotlkSDK::Discord::Activity;
    using DefaultFormatter = ::WotlkSDK::Discord::DefaultFormatter;

    inline bool START(std::string_view clientId = DISCORD_CLIENT_ID, uint32_t intervalMs = 5000) {
        return ::WotlkSDK::Discord::Start(clientId, intervalMs);
    }
    inline void STOP() {
        ::WotlkSDK::Discord::Stop();
    }
    inline void UPDATE() {
        ::WotlkSDK::Discord::Update();
    }
    inline bool IS_CONNECTED() {
        return ::WotlkSDK::Discord::IsConnected();
    }
    inline void SET_CALLBACK(::WotlkSDK::Discord::ActivityCallback callback) {
        ::WotlkSDK::Discord::SetCallback(std::move(callback));
    }
    template <typename Formatter>
    inline void SET_FORMATTER() {
        ::WotlkSDK::Discord::SetFormatter<Formatter>();
    }
}  // namespace DISCORD

}  // namespace WotlkSDK

namespace DISCORD = ::WotlkSDK::DISCORD;
