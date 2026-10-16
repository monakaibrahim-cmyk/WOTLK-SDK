/**
 * @file Main.cpp
 * @brief Reference implementation demonstrating Discord Social / Partner SDK and Rich Presence in WotlkSDK.
 *
 * Demonstrates:
 * - Dynamic Discord Activity state formatting (e.g., "Dungeon Queue", "Raid Queue", "In Dungeon")
 * - Party & Raid size integration (e.g., 1/4, 4/8, 5/5, 10/10, 25/25)
 * - Activity timestamps (activity start and queue completion / countdown ETA)
 * - Supported platforms configuration (Desktop, Android, iOS)
 * - Dual backend compatibility: Works out-of-the-box via native zero-dependency Windows Named Pipes
 *   or linked directly against Discord Social / Partner SDK (discord_partner_sdk.lib / discord_game_sdk.lib).
 */

#include <windows.h>
#include <chrono>
#include <format>
#include <string>
#include <WOTLK-SDK/wotlkcpp.h>

/**
 * @brief Custom Discord activity formatter demonstrating queue state, party size, timestamps, and platforms.
 */
struct ModDiscordFormatter {
    static void Format(const PRESENCE::RICH_PRESENCE_DATA& in, DISCORD::Activity& out) {
        // Base formatting from character level, race, class, and zone
        DISCORD::DefaultFormatter::Format(in, out);

        // Platform specification: Set to 1 (Desktop)
        out.supported_platforms = 1;

        if (in.is_in_world) {
            // Queue & Party demonstration:
            // Check if player is currently in a queue
            if (in.is_in_queue && in.queue_state && in.queue_state[0] != '\0') {
                out.state = in.queue_state; // e.g. "Dungeon Queue" or "Raid Queue"
            }

            // Party sizing: Display active party or raid member capacity (e.g. 1/4, 4/8)
            if (out.party_size == 0) {
                // If not in a group, default to 1/4 when queuing for dungeons or 4/8 for raid
                if (in.is_in_queue) {
                    out.party_size = 1;
                    out.party_max = 4;
                    out.party_id = "wotlk_dungeon_queue";
                }
            }
        }
    }
};

/**
 * @brief Simulates queueing for a Dungeon Finder or Raid instance.
 * @param queueName Queue state name (e.g., "Dungeon Queue", "Raid Queue").
 * @param partySize Current party count (e.g. 1, 4).
 * @param partyMax Maximum party capacity (e.g. 4, 8, 5, 25).
 * @param waitSeconds Estimated queue duration in seconds.
 */
void simulateQueueStatus(const char* queueName, uint32_t partySize, uint32_t partyMax, int64_t waitSeconds) {
    int64_t now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    int64_t eta = now + waitSeconds;

    // Use high-level ergonomic SDK helpers:
    DISCORD::SET_QUEUE(queueName, partySize, partyMax, eta);
    DISCORD::SET_SUPPORTED_PLATFORMS(1); // Desktop
    DISCORD::UPDATE();
}

/**
 * @brief DLL entry point for DiscordSDKExample mod.
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);

        // 1. Initialize WotlkSDK engine hooks
        if (!WotlkSDK::Initialize()) {
            return FALSE;
        }

        // 2. Install the custom Discord Activity Formatter
        DISCORD::SET_FORMATTER<ModDiscordFormatter>();

        // 3. (Optional) Example: Programmatically set queue and party state
        // Example 1: Dungeon Queue (1/4 members) with estimated ETA
        // simulateQueueStatus("Dungeon Queue", 1, 4, 180);
        // Example 2: Raid Queue (4/8 members)
        // simulateQueueStatus("Raid Queue", 4, 8, 300);

        // 4. Hook into world enter to trigger presence refresh
        WotlkSDK::ON_ENTER->add([]() {
            DISCORD::UPDATE();
        });

    } else if (reason == DLL_PROCESS_DETACH) {
        DISCORD::STOP();
        WotlkSDK::Shutdown();
    }

    return TRUE;
}
