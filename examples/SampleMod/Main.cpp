/**
 * @file Main.cpp
 * @brief Reference C++ mod implementation demonstrating WotlkSDK integration.
 *
 * Demonstrates:
 * - Engine lifecycle initialization and shutdown
 * - Per-frame render loop callbacks (ON_UPDATE)
 * - World enter/leave event dispatching (ON_ENTER, ON_LEAVE)
 * - Custom typed console variable registration with runtime callbacks
 * - Discord Rich Presence custom template formatter override
 */

#include <windows.h>
#include <WOTLK-SDK/wotlkcpp.h>

/**
 * @brief Frame update callback executing synchronously on each engine render tick.
 */
void onWorldUpdate() {
    if (GAME::IS_IN_WORLD()) {
        guid_t playerGuid = PLAYER::GET_UID();
        uint32_t activeSpellId = SPELL::GET_ID();
        const char* currentZone = ZONE::GET_ZONE_TEXT();
        (void)playerGuid;
        (void)activeSpellId;
        (void)currentZone;
    }
}

/**
 * @brief Dispatched when the local player character successfully enters the game world.
 */
void onWorldEnter() {
    guid_t playerGuid = PLAYER::GET_UID();
    const char* playerName = PLAYER::GET_NAME();
    uint32_t playerLevel = PLAYER::GET_LEVEL();
    (void)playerGuid;
    (void)playerName;
    (void)playerLevel;

    // Snapshot game and location state for diagnostics or presence
    auto presenceSnapshot = PRESENCE::GET_DATA();
    (void)presenceSnapshot;
}

/**
 * @brief Dispatched when the local player character leaves the game world or logs out.
 */
void onWorldLeave() {
}

/**
 * @brief Dynamic link library entry point for the mod.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // 1. (Optional) Programmatically configure auto-login credentials.
        // If omitted, AutoLogin checks CLI arguments (-login, -password),
        // environment variables (WOTLK_LOGIN, WOTLK_PASSWORD), or client WTF files.
        // WotlkSDK::AutoLogin::Configure("username", "password", "logon.example.com", "Lordaeron", "Character");

        // 2. Initialize SDK engine hooks and memory validation patches
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // 3. Register engine lifecycle event listeners
        WotlkSDK::ON_UPDATE->add(onWorldUpdate);
        WotlkSDK::ON_ENTER->add(onWorldEnter);
        WotlkSDK::ON_LEAVE->add(onWorldLeave);

        // 4. Register a custom console CVar with automatic range clamping and callbacks
        WotlkSDK::CVAR_REGISTRY->add<int>({
            .name = "sample_mod_enabled",
            .desc = "Toggles SampleMod features (1 = enabled, 0 = disabled)",
            .init = 1,
            .on_change = [](int newValue, bool changed) {
                if (changed) {
                    // Handle runtime CVar adjustment
                }
            }
        });

        // 5. (Optional) Install a custom Discord Rich Presence template formatter
        struct MyDiscordFormatter {
            static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, DISCORD::Activity& out) {
                DISCORD::DefaultFormatter::Format(in, out);
                out.details = std::format("[Modded] {}", out.details);
            }
        };
        DISCORD::SET_FORMATTER<MyDiscordFormatter>();

        // If DISCORD_RICH_PRESENCE and DISCORD_CLIENT_ID were set at compile time,
        // the presence thread starts automatically during Initialize().
        // Otherwise, it can be launched manually:
        // DISCORD::START("123456789012345678", 5000);
    } else if (reason == DLL_PROCESS_DETACH) {
        WotlkSDK::Shutdown();
    }

    return TRUE;
}
