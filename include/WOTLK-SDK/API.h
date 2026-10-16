#pragma once

/**
 * @file API.h
 * @brief High-level typed module APIs providing SCREAMING_SNAKE_CASE interfaces.
 *
 * Exposes organized subsystems for players, spells, game state, zones, objects,
 * console variables, Lua execution, auto-login, and rich presence querying.
 */

#include <cstdint>
#include <string_view>
#include <utility>

#include "Defines.h"
#include "../BaseTypes.h"
#include "../Constants.h"
#include "../ObjectManager/ObjectManager.h"
#include "../ObjectManager/CGObject_C.h"
#include "../ObjectManager/CGUnit_C.h"
#include "../ObjectManager/CGPlayer_C.h"
#include "../Spell/CSpell_C.h"
#include "../Glue/LoginUI.h"
#include "../Game/CGGameUI.h"
#include "../System/System.h"
#include "../FrameScript/FrameScript.h"
#include "AutoLogin.h"
#include "Extensions.h"

namespace WotlkSDK {

/* ========================================================================= */
/* PLAYER: Character, Unit State & Capabilities                              */
/* ========================================================================= */

/**
 * @brief Player character queries, targeting controls, and unit properties.
 */
namespace PLAYER {

    /**
     * @brief Retrieves the 64-bit globally unique identifier (GUID) of the active player character.
     * @return guid_t GUID of local player, or 0 if not currently logged into the game world.
     */
    inline guid_t GET_UID() {
        return ObjectMgr::getPlayerGuid();
    }

    /**
     * @brief Alias for GET_UID(). Retrieves the local player GUID.
     * @return guid_t Player GUID.
     */
    inline guid_t GET_GUID() {
        return GET_UID();
    }

    /**
     * @brief Retrieves the 64-bit GUID of the player's currently selected target.
     * @return guid_t Target GUID, or 0 if no entity is targeted.
     */
    inline guid_t GET_TARGET_UID() {
        return ObjectMgr::getTargetGuid();
    }

    /**
     * @brief Alias for GET_TARGET_UID(). Retrieves the active target GUID.
     * @return guid_t Target GUID.
     */
    inline guid_t GET_TARGET_GUID() {
        return GET_TARGET_UID();
    }

    /**
     * @brief Retrieves the CGUnit_C object instance pointer for the local player.
     * @return CGUnit_C* Pointer to player unit object, or nullptr if unavailable.
     */
    inline CGUnit_C* GET_UNIT() {
        guid_t guid = GET_UID();
        return guid ? ObjectMgr::get<CGUnit_C>(guid, eTypemaskPlayer) : nullptr;
    }

    /**
     * @brief Retrieves the CGPlayer_C object instance pointer for the local player.
     * @return CGPlayer_C* Pointer to player object, or nullptr if unavailable.
     */
    inline CGPlayer_C* GET_PLAYER() {
        guid_t guid = GET_UID();
        return guid ? ObjectMgr::get<CGPlayer_C>(guid, eTypemaskPlayer) : nullptr;
    }

    /**
     * @brief Retrieves the active character name.
     *
     * Resolves the name from in-world player memory if logged in, or from the
     * glue screen character selection list if currently at the character select menu.
     *
     * @return const char* Character name string, or empty string if unavailable.
     */
    inline const char* GET_NAME() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* name = reinterpret_cast<const char*>(0x00C79D18);
            if (name && name[0] != '\0') {
                return name;
            }
            auto* unit = GET_UNIT();
            if (unit) {
                const char* unitName = unit->getName(nullptr, 0);
                if (unitName && unitName[0] != '\0') {
                    return unitName;
                }
            }
        } else if (*reinterpret_cast<const uint32_t*>(0x00B6A9E0) == 1) {
            int idx = *reinterpret_cast<const int*>(0x00AC436C);
            auto& tsgra = Login::CharacterSelectionDisplay::TSGRA;
            if (idx >= 0 && static_cast<uint32_t>(idx) < tsgra.count && tsgra.data) {
                return tsgra.data[idx].name;
            }
        }
        return "";
    }

    /**
     * @brief Retrieves the player's current character level (1-80).
     * @return uint32_t Character level, or 0 if unavailable.
     */
    inline uint32_t GET_LEVEL() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return unit->descriptors_->level;
            }
        } else if (*reinterpret_cast<const uint32_t*>(0x00B6A9E0) == 1) {
            int idx = *reinterpret_cast<const int*>(0x00AC436C);
            auto& tsgra = Login::CharacterSelectionDisplay::TSGRA;
            if (idx >= 0 && static_cast<uint32_t>(idx) < tsgra.count && tsgra.data) {
                return static_cast<uint8_t>(tsgra.data[idx].level);
            }
        }
        return 0;
    }

    /**
     * @brief Retrieves the character race identifier ID.
     * @return uint8_t Numeric race ID (1=Human, 2=Orc, 3=Dwarf, etc.).
     */
    inline uint8_t GET_RACE() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return unit->descriptors_->bytes0[0];
            }
        } else if (*reinterpret_cast<const uint32_t*>(0x00B6A9E0) == 1) {
            int idx = *reinterpret_cast<const int*>(0x00AC436C);
            auto& tsgra = Login::CharacterSelectionDisplay::TSGRA;
            if (idx >= 0 && static_cast<uint32_t>(idx) < tsgra.count && tsgra.data) {
                return static_cast<uint8_t>(tsgra.data[idx].race);
            }
        }
        return 0;
    }

    /**
     * @brief Resolves a numeric race ID into its canonical display string.
     * @param raceId Numeric race ID.
     * @return const char* Human-readable race name (e.g., "Human", "Night Elf").
     */
    inline const char* GET_RACE_NAME(uint8_t raceId) {
        switch (raceId) {
            case 1:  return "Human";
            case 2:  return "Orc";
            case 3:  return "Dwarf";
            case 4:  return "Night Elf";
            case 5:  return "Undead";
            case 6:  return "Tauren";
            case 7:  return "Gnome";
            case 8:  return "Troll";
            case 9:  return "Goblin";
            case 10: return "Blood Elf";
            case 11: return "Draenei";
            default: return "Unknown";
        }
    }

    /**
     * @brief Retrieves the active character's canonical race name string.
     * @return const char* Race name (e.g., "Orc", "Blood Elf").
     */
    inline const char* GET_RACE_NAME() {
        return GET_RACE_NAME(GET_RACE());
    }

    /**
     * @brief Retrieves the character class identifier ID.
     * @return uint8_t Numeric class ID (1=Warrior, 2=Paladin, 3=Hunter, etc.).
     */
    inline uint8_t GET_CLASS() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return unit->descriptors_->bytes0[1];
            }
        } else if (*reinterpret_cast<const uint32_t*>(0x00B6A9E0) == 1) {
            int idx = *reinterpret_cast<const int*>(0x00AC436C);
            auto& tsgra = Login::CharacterSelectionDisplay::TSGRA;
            if (idx >= 0 && static_cast<uint32_t>(idx) < tsgra.count && tsgra.data) {
                return static_cast<uint8_t>(tsgra.data[idx].char_class);
            }
        }
        return 0;
    }

    /**
     * @brief Resolves a numeric class ID into its canonical display string.
     * @param classId Numeric class ID.
     * @return const char* Human-readable class name (e.g., "Death Knight", "Mage").
     */
    inline const char* GET_CLASS_NAME(uint8_t classId) {
        switch (classId) {
            case 1:  return "Warrior";
            case 2:  return "Paladin";
            case 3:  return "Hunter";
            case 4:  return "Rogue";
            case 5:  return "Priest";
            case 6:  return "Death Knight";
            case 7:  return "Shaman";
            case 8:  return "Mage";
            case 9:  return "Warlock";
            case 11: return "Druid";
            default: return "Unknown";
        }
    }

    /**
     * @brief Retrieves the active character's canonical class name string.
     * @return const char* Class name (e.g., "Paladin").
     */
    inline const char* GET_CLASS_NAME() {
        return GET_CLASS_NAME(GET_CLASS());
    }

    /**
     * @brief Retrieves the character gender ID.
     * @return uint8_t 0 for Male, 1 for Female.
     */
    inline uint8_t GET_GENDER() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return unit->descriptors_->bytes0[2];
            }
        } else if (*reinterpret_cast<const uint32_t*>(0x00B6A9E0) == 1) {
            int idx = *reinterpret_cast<const int*>(0x00AC436C);
            auto& tsgra = Login::CharacterSelectionDisplay::TSGRA;
            if (idx >= 0 && static_cast<uint32_t>(idx) < tsgra.count && tsgra.data) {
                return static_cast<uint8_t>(tsgra.data[idx].gender);
            }
        }
        return 0;
    }

    /**
     * @brief Retrieves the character gender display name.
     * @return const char* "Female" or "Male".
     */
    inline const char* GET_GENDER_NAME() {
        return GET_GENDER() == 1 ? "Female" : "Male";
    }

    /**
     * @brief Retrieves the player's current health points.
     * @return uint32_t Current health value.
     */
    inline uint32_t GET_HEALTH() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? unit->descriptors_->health : 0;
    }

    /**
     * @brief Retrieves the player's maximum health points.
     * @return uint32_t Maximum health value.
     */
    inline uint32_t GET_MAX_HEALTH() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? unit->descriptors_->maxhealth : 0;
    }

    /**
     * @brief Retrieves current power resource points for a given power type index.
     * @param index Power type index (0=Mana, 1=Rage, 2=Focus, 3=Energy, 6=Runic Power).
     * @return uint32_t Current power value.
     */
    inline uint32_t GET_POWER(uint32_t index = 0) {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && index < 7) ? unit->descriptors_->power[index] : 0;
    }

    /**
     * @brief Retrieves maximum power resource points for a given power type index.
     * @param index Power type index.
     * @return uint32_t Maximum power value.
     */
    inline uint32_t GET_MAX_POWER(uint32_t index = 0) {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && index < 7) ? unit->descriptors_->maxpower[index] : 0;
    }

    /**
     * @brief Retrieves current mana points.
     * @return uint32_t Mana value.
     */
    inline uint32_t GET_MANA() {
        return GET_POWER(0);
    }

    /**
     * @brief Retrieves maximum mana points.
     * @return uint32_t Maximum mana value.
     */
    inline uint32_t GET_MAX_MANA() {
        return GET_MAX_POWER(0);
    }

    /**
     * @brief Retrieves 3D world coordinates of the player.
     * @return Vec3f 3D position vector {x, y, z}.
     */
    inline Vec3f GET_POSITION() {
        auto* unit = GET_UNIT();
        return unit ? unit->movement_.mov_pos_ : Vec3f{0.0f, 0.0f, 0.0f};
    }

    /**
     * @brief Retrieves the player's facing angle in radians.
     * @return float Facing orientation in radians [0, 2*PI).
     */
    inline float GET_FACING() {
        auto* unit = GET_UNIT();
        return unit ? unit->movement_.facing_ : 0.0f;
    }

    /**
     * @brief Checks if the player is currently engaged in combat lockdown.
     * @return true if in combat lockdown, false otherwise.
     */
    inline bool IS_IN_COMBAT() {
        if (GameUI::inCombatLockdown()) { return true; }
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && (unit->descriptors_->flags & 0x00080000));
    }

    /**
     * @brief Checks if the player is dead.
     * @return true if health equals 0, false otherwise.
     */
    inline bool IS_DEAD() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? (unit->descriptors_->health == 0) : false;
    }

    /**
     * @brief Checks if the player is in ghost form after releasing spirit.
     * @return true if dead in ghost form, false otherwise.
     */
    inline bool IS_GHOST() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? ((unit->descriptors_->flags & 0x00000010) != 0) : false;
    }

    /**
     * @brief Checks if the player is alive (not dead and not in ghost form).
     * @return true if alive, false otherwise.
     */
    inline bool IS_ALIVE() {
        return !IS_DEAD() && !IS_GHOST();
    }

    /**
     * @brief Sets the game client's active target to the specified entity GUID.
     * @param guid 64-bit entity GUID to target.
     */
    inline void SET_TARGET(guid_t guid) {
        GameUI::target{}(guid);
    }

    /**
     * @brief Clears the active target selection.
     */
    inline void CLEAR_TARGET() {
        GameUI::clearTarget{}(0, 0);
    }

}  // namespace PLAYER

/* ========================================================================= */
/* SPELL: Spell Casting, Pending Spells, & AoE                               */
/* ========================================================================= */

/**
 * @brief Spell casting queries, targeting states, and spell executions.
 */
namespace SPELL {

    /**
     * @brief Retrieves the spell ID of the currently pending or casting spell.
     * @return uint32_t Spell ID, or 0 if no spell is currently being cast.
     */
    inline uint32_t GET_ID() {
        return CSpell_C::PENDING_SPELL_CAST ? CSpell_C::PENDING_SPELL_CAST->spell_id : 0;
    }

    /**
     * @brief Checks if the cursor is currently in area-of-effect (AoE) targeting reticle mode.
     * @return true if AoE targeting mode is active, false otherwise.
     */
    inline bool IS_TARGETING_AOE() {
        return CSpell_C::isTargetingAoE{}();
    }

    /**
     * @brief Cancels pending area-of-effect targeting reticle mode.
     * @return int Engine status code.
     */
    inline int CANCEL_AOE_TARGETING() {
        return CSpell_C::cancelPendingAoeTargeting{}();
    }

    /**
     * @brief Initiates casting of a spell by ID.
     * @param spellId Numeric spell identifier.
     * @param item Optional source item instance pointer.
     * @param target Optional destination target entity GUID.
     * @return int Cast execution status code.
     */
    inline int CAST(uint32_t spellId, CGItem_C* item = nullptr, guid_t target = 0) {
        guid_t playerGuid = ObjectMgr::getPlayerGuid();
        auto* player = playerGuid ? ObjectMgr::get<CGUnit_C>(playerGuid, eTypemaskPlayer) : nullptr;
        return CSpell_C::castSpell{}(player, spellId, item, target, nullptr, false);
    }

}  // namespace SPELL

/* ========================================================================= */
/* GAME: Engine Lifecycle, Game State, & Window Queries                      */
/* ========================================================================= */

/**
 * @brief Client lifecycle states, game window handle, and timing queries.
 */
namespace GAME {

    /**
     * @brief Retrieves the numeric engine glue/game state code.
     * @return uint32_t State ID (0=Login Screen, 1=Character Selection, 2=In Game).
     */
    inline uint32_t GET_STATE() {
        return *reinterpret_cast<const uint32_t*>(0x00B6A9E0);
    }

    /**
     * @brief Checks if the local player is fully loaded and present in the game world.
     * @return true if in world, false if at login, character select, or loading screen.
     */
    inline bool IS_IN_WORLD() {
        return *reinterpret_cast<const char*>(0x00BD0792) != 0;
    }

    /**
     * @brief Checks if an engine loading screen is currently being rendered.
     * @return true if loading screen is active, false otherwise.
     */
    inline bool IS_LOADING() {
        return *reinterpret_cast<const uint32_t*>(0x00B6AA38) != 0;
    }

    /**
     * @brief Retrieves the display name of the current engine state.
     * @return const char* State description ("In Game", "Character Selection", "Login Screen", etc.).
     */
    inline const char* GET_STATE_NAME() {
        if (IS_LOADING()) {
            return "Loading Screen";
        }
        switch (GET_STATE()) {
            case 0:  return "Login Screen";
            case 1:  return "Character Selection";
            case 2:  return "In Game";
            default: return "Unknown";
        }
    }

    /**
     * @brief Retrieves the primary OS window handle (HWND) for the game client.
     * @return void* HWND window handle cast to void*.
     */
    inline void* GET_WINDOW() {
        return reinterpret_cast<void*>(OS::getGameWindow());
    }

    /**
     * @brief Retrieves high-resolution asynchronous millisecond tick counter.
     * @return uint64_t Timestamp in milliseconds.
     */
    inline uint64_t GET_TIME_MS() {
        return OS::getAsyncTimeMs{}();
    }

}  // namespace GAME

/* ========================================================================= */
/* ZONE: Current Area, Subzone, Minimap, Map & Continent                     */
/* ========================================================================= */

/**
 * @brief Geographic location, zone, subzone, and map queries.
 */
namespace ZONE {

    /**
     * @brief Retrieves the main zone text displayed on screen (e.g., "Dalaran").
     * @return const char* Zone text string.
     */
    inline const char* GET_ZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD0788);
        return (ptr && *ptr) ? *ptr : "";
    }

    /**
     * @brief Alias for GET_ZONE_TEXT(). Retrieves current zone text.
     * @return const char* Zone text.
     */
    inline const char* GET_TEXT() {
        return GET_ZONE_TEXT();
    }

    /**
     * @brief Retrieves subzone text (e.g., "Runeweaver Square").
     * @return const char* Subzone text string.
     */
    inline const char* GET_SUBZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD0784);
        return (ptr && *ptr) ? *ptr : "";
    }

    /**
     * @brief Retrieves minimap hover zone text.
     * @return const char* Minimap zone text.
     */
    inline const char* GET_MINIMAP_ZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD077C);
        return (ptr && *ptr) ? *ptr : "";
    }

    /**
     * @brief Retrieves the numeric area table ID for the current location.
     * @return uint32_t Area ID from AreaTable.dbc.
     */
    inline uint32_t GET_AREA_ID() {
        auto* ptr = reinterpret_cast<const uint32_t*>(0x00BD080C);
        return ptr ? *ptr : 0;
    }

    /**
     * @brief Retrieves the numeric Map ID.
     * @return uint32_t Map ID from Map.dbc (0=Eastern Kingdoms, 1=Kalimdor, 530=Outland, 571=Northrend).
     */
    inline uint32_t GET_MAP_ID() {
        auto* ptr = reinterpret_cast<const uint32_t*>(0x00AB63BC);
        return ptr ? *ptr : 0;
    }

    /**
     * @brief Retrieves internal map folder / directory name.
     * @return const char* Map identifier string (e.g., "Northrend").
     */
    inline const char* GET_MAP_NAME() {
        auto** ptr = reinterpret_cast<const char**>(0x00CE06D0);
        return (ptr && *ptr) ? *ptr : "";
    }

    /**
     * @brief Maps a numeric Map ID to its canonical continent name.
     * @param mapId Numeric map ID.
     * @return const char* Continent name (e.g., "Northrend", "Eastern Kingdoms").
     */
    inline const char* GET_CONTINENT_NAME(uint32_t mapId) {
        switch (mapId) {
            case 0:   return "Eastern Kingdoms";
            case 1:   return "Kalimdor";
            case 530: return "Outland";
            case 571: return "Northrend";
            default:  return "Azeroth";
        }
    }

    /**
     * @brief Retrieves the continent name for the player's current map.
     * @return const char* Continent name.
     */
    inline const char* GET_CONTINENT_NAME() {
        return GET_CONTINENT_NAME(GET_MAP_ID());
    }

}  // namespace ZONE

/* ========================================================================= */
/* OBJECT: Object Manager Lookups & Enumeration                              */
/* ========================================================================= */

/**
 * @brief Direct object manager lookups, casting, and entity enumeration.
 */
namespace OBJECT {

    /**
     * @brief Looks up an entity by GUID from the engine object manager table.
     * @param guid 64-bit entity GUID.
     * @param mask Entity type mask filter.
     * @return CGObject_C* Base object pointer, or nullptr if not found.
     */
    inline CGObject_C* GET(guid_t guid, TypeMask mask = static_cast<TypeMask>(0xFF)) {
        return ObjectMgr::get<CGObject_C>(guid, mask);
    }

    /**
     * @brief Looks up a unit (creature or player) by GUID.
     * @param guid 64-bit unit GUID.
     * @return CGUnit_C* Unit pointer, or nullptr if not found.
     */
    inline CGUnit_C* GET_UNIT(guid_t guid) {
        return ObjectMgr::get<CGUnit_C>(guid, eTypemaskUnit);
    }

    /**
     * @brief Looks up a player by GUID.
     * @param guid 64-bit player GUID.
     * @return CGPlayer_C* Player pointer, or nullptr if not found.
     */
    inline CGPlayer_C* GET_PLAYER(guid_t guid) {
        return ObjectMgr::get<CGPlayer_C>(guid, eTypemaskPlayer);
    }

    /**
     * @brief Enumerates all active objects in local memory matching an optional predicate.
     * @tparam F Callable receiving (CGObject_C* object, void* context).
     * @param callback Visitor functor returning true to continue enumeration, false to halt.
     * @return true on normal completion, false if interrupted early.
     */
    template <typename F>
    inline bool ENUM_OBJECTS(F&& callback) {
        return ObjectMgr::enumObjects(std::forward<F>(callback));
    }

}  // namespace OBJECT

/* ========================================================================= */
/* PRESENCE: Rich Presence Data Snapshot                                     */
/* ========================================================================= */

/**
 * @brief Comprehensive snapshot generation for social rich presence integrators.
 */
namespace PRESENCE {

    /**
     * @brief Unified snapshot of live engine, player, and geographic state.
     */
    struct RICH_PRESENCE_DATA {
        uint32_t    game_state;      /**< Numeric engine state code (0=Login, 1=CharSelect, 2=InGame). */
        const char* game_state_name; /**< Human-readable engine state name. */
        const char* character_name;  /**< Active or selected character name. */
        uint32_t    level;           /**< Character level (1-80). */
        const char* race_name;       /**< Character race display string. */
        const char* class_name;      /**< Character class display string. */
        const char* zone_text;       /**< Current zone name. */
        const char* subzone_text;    /**< Current subzone name. */
        const char* continent_name;  /**< Current continent name. */
        const char* map_name;        /**< Internal map directory name. */
        uint32_t    map_id;          /**< Numeric Map ID from Map.dbc. */
        uint32_t    area_id;         /**< Numeric Area ID from AreaTable.dbc. */
        bool        is_in_world;     /**< true if actively present in the game world. */
        bool        is_loading;      /**< true if an engine loading screen is rendering. */
        bool        is_in_combat;    /**< true if player is currently in combat. */
    };

    /**
     * @brief Captures an atomic snapshot of current client state suitable for rich presence formatting.
     * @return RICH_PRESENCE_DATA Populated snapshot structure.
     */
    inline RICH_PRESENCE_DATA GET_DATA() {
        RICH_PRESENCE_DATA data{};
        data.game_state = GAME::GET_STATE();
        data.is_in_world = GAME::IS_IN_WORLD();
        data.is_loading = GAME::IS_LOADING();
        data.game_state_name = GAME::GET_STATE_NAME();

        if (data.is_in_world) {
            data.character_name = PLAYER::GET_NAME();
            data.level = PLAYER::GET_LEVEL();
            data.race_name = PLAYER::GET_RACE_NAME();
            data.class_name = PLAYER::GET_CLASS_NAME();
            data.zone_text = ZONE::GET_ZONE_TEXT();
            data.subzone_text = ZONE::GET_SUBZONE_TEXT();
            data.map_id = ZONE::GET_MAP_ID();
            data.area_id = ZONE::GET_AREA_ID();
            data.map_name = ZONE::GET_MAP_NAME();
            data.continent_name = ZONE::GET_CONTINENT_NAME(data.map_id);
            data.is_in_combat = PLAYER::IS_IN_COMBAT();
        } else if (data.game_state == 1) {
            data.character_name = PLAYER::GET_NAME();
            data.level = PLAYER::GET_LEVEL();
            data.race_name = PLAYER::GET_RACE_NAME();
            data.class_name = PLAYER::GET_CLASS_NAME();
            data.zone_text = "Character Selection";
            data.subzone_text = "";
            data.continent_name = "";
            data.map_name = "";
            data.map_id = 0;
            data.area_id = 0;
            data.is_in_combat = false;
        } else {
            data.character_name = "";
            data.level = 0;
            data.race_name = "";
            data.class_name = "";
            data.zone_text = data.game_state_name;
            data.subzone_text = "";
            data.continent_name = "";
            data.map_name = "";
            data.map_id = 0;
            data.area_id = 0;
            data.is_in_combat = false;
        }
        return data;
    }

}  // namespace PRESENCE

/* ========================================================================= */
/* CVAR: Console Variable Read/Write                                         */
/* ========================================================================= */

/**
 * @brief Console variable queries and programmatic updates.
 */
namespace CVAR {

    /**
     * @brief Reads an integer console variable value.
     * @param name CVar name string.
     * @return int Parsed integer value.
     */
    inline int GET_INT(const char* name) {
        return ::Extensions::Console::CVAR_REGISTRY->get<int>(name ? name : "");
    }

    /**
     * @brief Reads a floating point console variable value.
     * @param name CVar name string.
     * @return float Parsed float value.
     */
    inline float GET_FLOAT(const char* name) {
        return ::Extensions::Console::CVAR_REGISTRY->get<float>(name ? name : "");
    }

    /**
     * @brief Writes a string value to a console variable.
     * @param name CVar name string.
     * @param value String representation of the new value.
     */
    inline void SET(const char* name, const char* value) {
        ::Extensions::Console::CVAR_REGISTRY->set(name ? name : "", value ? value : "");
    }

    /**
     * @brief Triggers a refresh notification for a console variable.
     * @param name CVar name string.
     */
    inline void REFRESH(const char* name) {
        ::Extensions::Console::CVAR_REGISTRY->refresh(name ? name : "");
    }

}  // namespace CVAR

/* ========================================================================= */
/* LUA: In-Game Script Execution                                             */
/* ========================================================================= */

/**
 * @brief In-engine Lua 5.1 script string execution.
 */
namespace LUA {

    /**
     * @brief Executes a snippet of Lua code within the game client UI environment.
     * @param script Null-terminated Lua script buffer.
     * @return int Execution result code (1 on success, 0 on failure).
     */
    inline int EXECUTE(const char* script) {
        if (!script) { return 0; }
        return FrameScript::execute{}(script, script, nullptr);
    }

}  // namespace LUA

/* ========================================================================= */
/* AUTOLOGIN: Programmatic Configuration & Accessors                         */
/* ========================================================================= */

/**
 * @brief Programmatic auto-login credentials and realm destination configuration.
 */
namespace AUTOLOGIN {

    /**
     * @brief Configures credentials and destination realm options.
     * @param login Account username.
     * @param password Account password.
     * @param realmlist Optional realmlist host override.
     * @param realm_name Optional destination realm name.
     * @param character Optional character name to select upon realm entry.
     */
    inline void CONFIGURE(
        const char* login,
        const char* password,
        const char* realmlist,
        const char* realm_name,
        const char* character
    ) {
        ::AutoLogin::Configure(
            login ? login : "",
            password ? password : "",
            realmlist ? realmlist : "",
            realm_name ? realm_name : "",
            character ? character : ""
        );
    }

    /**
     * @brief Sets account username.
     * @param login Account username string.
     */
    inline void SET_LOGIN(const char* login) {
        ::AutoLogin::SetLogin(login ? login : "");
    }

    /**
     * @brief Sets account password.
     * @param password Account password string.
     */
    inline void SET_PASSWORD(const char* password) {
        ::AutoLogin::SetPassword(password ? password : "");
    }

    /**
     * @brief Sets realmlist host override.
     * @param realmlist Hostname or IP string.
     */
    inline void SET_REALMLIST(const char* realmlist) {
        ::AutoLogin::SetRealmlist(realmlist ? realmlist : "");
    }

    /**
     * @brief Sets target realm name.
     * @param realm_name Realm name string.
     */
    inline void SET_REALM_NAME(const char* realm_name) {
        ::AutoLogin::SetRealmName(realm_name ? realm_name : "");
    }

    /**
     * @brief Sets destination character name for automatic selection.
     * @param character Character name string.
     */
    inline void SET_CHARACTER(const char* character) {
        ::AutoLogin::SetCharacter(character ? character : "");
    }

    /**
     * @brief Retrieves configured account username.
     * @return std::string_view Username string view.
     */
    inline std::string_view GET_LOGIN() {
        return ::AutoLogin::GetLogin();
    }

    /**
     * @brief Retrieves configured account password.
     * @return std::string_view Password string view.
     */
    inline std::string_view GET_PASSWORD() {
        return ::AutoLogin::GetPassword();
    }

    /**
     * @brief Retrieves configured realmlist host.
     * @return std::string_view Realmlist string view.
     */
    inline std::string_view GET_REALMLIST() {
        return ::AutoLogin::GetRealmlist();
    }

    /**
     * @brief Retrieves configured realm name.
     * @return std::string_view Realm name string view.
     */
    inline std::string_view GET_REALM_NAME() {
        return ::AutoLogin::GetRealmName();
    }

    /**
     * @brief Retrieves configured character name.
     * @return std::string_view Character name string view.
     */
    inline std::string_view GET_CHARACTER() {
        return ::AutoLogin::GetCharacter();
    }

}  // namespace AUTOLOGIN

}  // namespace WotlkSDK

/* ========================================================================= */
/* Global Ergonomic Aliases: Direct Type Namespace Access                    */
/* Allows writing PLAYER::GET_UID(), SPELL::GET_ID(), etc. directly          */
/* ========================================================================= */
namespace PLAYER = ::WotlkSDK::PLAYER;
namespace SPELL = ::WotlkSDK::SPELL;
namespace GAME = ::WotlkSDK::GAME;
namespace ZONE = ::WotlkSDK::ZONE;
namespace OBJECT = ::WotlkSDK::OBJECT;
namespace PRESENCE = ::WotlkSDK::PRESENCE;
namespace CVAR = ::WotlkSDK::CVAR;
namespace LUA = ::WotlkSDK::LUA;
namespace AUTOLOGIN = ::WotlkSDK::AUTOLOGIN;
