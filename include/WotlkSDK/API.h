#pragma once

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
namespace PLAYER {

    inline guid_t GET_UID() {
        return ObjectMgr::getPlayerGuid();
    }

    inline guid_t GET_GUID() {
        return GET_UID();
    }

    inline guid_t GET_TARGET_UID() {
        return ObjectMgr::getTargetGuid();
    }

    inline guid_t GET_TARGET_GUID() {
        return GET_TARGET_UID();
    }

    inline CGUnit_C* GET_UNIT() {
        guid_t guid = GET_UID();
        return guid ? ObjectMgr::get<CGUnit_C>(guid, TypeMask::Player) : nullptr;
    }

    inline CGPlayer_C* GET_PLAYER() {
        guid_t guid = GET_UID();
        return guid ? ObjectMgr::get<CGPlayer_C>(guid, TypeMask::Player) : nullptr;
    }

    inline const char* GET_NAME() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* name = reinterpret_cast<const char*>(0x00C79D18);
            if (name && name[0] != '\0') {
                return name;
            }
            auto* unit = GET_UNIT();
            if (unit) {
                const char* unitName = unit->getName();
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

    inline uint8_t GET_RACE() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return static_cast<uint8_t>(unit->descriptors_->bytes0 & 0xFF);
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

    inline const char* GET_RACE_NAME() {
        return GET_RACE_NAME(GET_RACE());
    }

    inline uint8_t GET_CLASS() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return static_cast<uint8_t>((unit->descriptors_->bytes0 >> 8) & 0xFF);
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

    inline const char* GET_CLASS_NAME() {
        return GET_CLASS_NAME(GET_CLASS());
    }

    inline uint8_t GET_GENDER() {
        if (*reinterpret_cast<const char*>(0x00BD0792) != 0) {
            auto* unit = GET_UNIT();
            if (unit && unit->descriptors_) {
                return static_cast<uint8_t>((unit->descriptors_->bytes0 >> 16) & 0xFF);
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

    inline const char* GET_GENDER_NAME() {
        return GET_GENDER() == 1 ? "Female" : "Male";
    }

    inline uint32_t GET_HEALTH() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? unit->descriptors_->health : 0;
    }

    inline uint32_t GET_MAX_HEALTH() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? unit->descriptors_->maxHealth : 0;
    }

    inline uint32_t GET_POWER(uint32_t index = 0) {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && index < 7) ? unit->descriptors_->power[index] : 0;
    }

    inline uint32_t GET_MAX_POWER(uint32_t index = 0) {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && index < 7) ? unit->descriptors_->maxPower[index] : 0;
    }

    inline uint32_t GET_MANA() {
        return GET_POWER(0);
    }

    inline uint32_t GET_MAX_MANA() {
        return GET_MAX_POWER(0);
    }

    inline Vec3f GET_POSITION() {
        auto* unit = GET_UNIT();
        return unit ? unit->movement.position : Vec3f{0.0f, 0.0f, 0.0f};
    }

    inline float GET_FACING() {
        auto* unit = GET_UNIT();
        return unit ? unit->movement.facing : 0.0f;
    }

    inline bool IS_IN_COMBAT() {
        if (GameUI::inCombatLockdown()) { return true; }
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_ && (unit->descriptors_->flags & 0x00080000));
    }

    inline bool IS_DEAD() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? (unit->descriptors_->health == 0) : false;
    }

    inline bool IS_GHOST() {
        auto* unit = GET_UNIT();
        return (unit && unit->descriptors_) ? ((unit->descriptors_->flags & 0x00000010) != 0) : false;
    }

    inline bool IS_ALIVE() {
        return !IS_DEAD() && !IS_GHOST();
    }

    inline void SET_TARGET(guid_t guid) {
        GameUI::target{}(guid);
    }

    inline void CLEAR_TARGET() {
        GameUI::clearTarget{}(0, 0);
    }

}  // namespace PLAYER

/* ========================================================================= */
/* SPELL: Spell Casting, Pending Spells, & AoE                               */
/* ========================================================================= */
namespace SPELL {

    inline uint32_t GET_ID() {
        return CSpell_C::PENDING_SPELL_CAST ? CSpell_C::PENDING_SPELL_CAST->spell_id : 0;
    }

    inline bool IS_TARGETING_AOE() {
        return CSpell_C::isTargetingAoE{}();
    }

    inline int CANCEL_AOE_TARGETING() {
        return CSpell_C::cancelPendingAoeTargeting{}();
    }

    inline int CAST(uint32_t spellId, CGItem_C* item = nullptr, guid_t target = 0) {
        guid_t playerGuid = ObjectMgr::getPlayerGuid();
        auto* player = playerGuid ? ObjectMgr::get<CGUnit_C>(playerGuid, TypeMask::Player) : nullptr;
        return CSpell_C::castSpell{}(player, spellId, item, target, nullptr, false);
    }

}  // namespace SPELL

/* ========================================================================= */
/* GAME: Engine Lifecycle, Game State, & Window Queries                      */
/* ========================================================================= */
namespace GAME {

    inline uint32_t GET_STATE() {
        return *reinterpret_cast<const uint32_t*>(0x00B6A9E0);
    }

    inline bool IS_IN_WORLD() {
        return *reinterpret_cast<const char*>(0x00BD0792) != 0;
    }

    inline bool IS_LOADING() {
        return *reinterpret_cast<const uint32_t*>(0x00B6AA38) != 0;
    }

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

    inline void* GET_WINDOW() {
        return reinterpret_cast<void*>(OS::getGameWindow());
    }

    inline uint64_t GET_TIME_MS() {
        return OS::getAsyncTimeMs{}();
    }

}  // namespace GAME

/* ========================================================================= */
/* ZONE: Current Area, Subzone, Minimap, Map & Continent                     */
/* ========================================================================= */
namespace ZONE {

    inline const char* GET_ZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD0788);
        return (ptr && *ptr) ? *ptr : "";
    }

    inline const char* GET_TEXT() {
        return GET_ZONE_TEXT();
    }

    inline const char* GET_SUBZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD0784);
        return (ptr && *ptr) ? *ptr : "";
    }

    inline const char* GET_MINIMAP_ZONE_TEXT() {
        auto** ptr = reinterpret_cast<const char**>(0x00BD077C);
        return (ptr && *ptr) ? *ptr : "";
    }

    inline uint32_t GET_AREA_ID() {
        auto* ptr = reinterpret_cast<const uint32_t*>(0x00BD080C);
        return ptr ? *ptr : 0;
    }

    inline uint32_t GET_MAP_ID() {
        auto* ptr = reinterpret_cast<const uint32_t*>(0x00AB63BC);
        return ptr ? *ptr : 0;
    }

    inline const char* GET_MAP_NAME() {
        auto** ptr = reinterpret_cast<const char**>(0x00CE06D0);
        return (ptr && *ptr) ? *ptr : "";
    }

    inline const char* GET_CONTINENT_NAME(uint32_t mapId) {
        switch (mapId) {
            case 0:   return "Eastern Kingdoms";
            case 1:   return "Kalimdor";
            case 530: return "Outland";
            case 571: return "Northrend";
            default:  return "Azeroth";
        }
    }

    inline const char* GET_CONTINENT_NAME() {
        return GET_CONTINENT_NAME(GET_MAP_ID());
    }

}  // namespace ZONE

/* ========================================================================= */
/* OBJECT: Object Manager Lookups & Enumeration                              */
/* ========================================================================= */
namespace OBJECT {

    inline CGObject_C* GET(guid_t guid, TypeMask mask = TypeMask::All) {
        return ObjectMgr::get<CGObject_C>(guid, mask);
    }

    inline CGUnit_C* GET_UNIT(guid_t guid) {
        return ObjectMgr::get<CGUnit_C>(guid, TypeMask::Unit);
    }

    inline CGPlayer_C* GET_PLAYER(guid_t guid) {
        return ObjectMgr::get<CGPlayer_C>(guid, TypeMask::Player);
    }

    template <typename F>
    inline bool ENUM_OBJECTS(F&& callback) {
        return ObjectMgr::enumObjects(std::forward<F>(callback));
    }

}  // namespace OBJECT

/* ========================================================================= */
/* PRESENCE: Rich Presence Data Snapshot                                     */
/* ========================================================================= */
namespace PRESENCE {

    struct RICH_PRESENCE_DATA {
        uint32_t    game_state;
        const char* game_state_name;
        const char* character_name;
        uint32_t    level;
        const char* race_name;
        const char* class_name;
        const char* zone_text;
        const char* subzone_text;
        const char* continent_name;
        const char* map_name;
        uint32_t    map_id;
        uint32_t    area_id;
        bool        is_in_world;
        bool        is_loading;
        bool        is_in_combat;
    };

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
namespace CVAR {

    inline int GET_INT(const char* name) {
        return WotlkSDK::CVAR_REGISTRY->get<int>(name ? name : "");
    }

    inline float GET_FLOAT(const char* name) {
        return WotlkSDK::CVAR_REGISTRY->get<float>(name ? name : "");
    }

    inline void SET(const char* name, const char* value) {
        WotlkSDK::CVAR_REGISTRY->set(name ? name : "", value ? value : "");
    }

    inline void REFRESH(const char* name) {
        WotlkSDK::CVAR_REGISTRY->refresh(name ? name : "");
    }

}  // namespace CVAR

/* ========================================================================= */
/* LUA: In-Game Script Execution                                             */
/* ========================================================================= */
namespace LUA {

    inline int EXECUTE(const char* script) {
        if (!script) { return 0; }
        return FrameScript::execute{}(script, script, nullptr);
    }

}  // namespace LUA

/* ========================================================================= */
/* AUTOLOGIN: Programmatic Configuration & Accessors                         */
/* ========================================================================= */
namespace AUTOLOGIN {

    inline void CONFIGURE(
        const char* login,
        const char* password,
        const char* realmlist,
        const char* realm_name,
        const char* character
    ) {
        WotlkSDK::AutoLogin::Configure(
            login ? login : "",
            password ? password : "",
            realmlist ? realmlist : "",
            realm_name ? realm_name : "",
            character ? character : ""
        );
    }

    inline void SET_LOGIN(const char* login) {
        WotlkSDK::AutoLogin::SetLogin(login ? login : "");
    }

    inline void SET_PASSWORD(const char* password) {
        WotlkSDK::AutoLogin::SetPassword(password ? password : "");
    }

    inline void SET_REALMLIST(const char* realmlist) {
        WotlkSDK::AutoLogin::SetRealmlist(realmlist ? realmlist : "");
    }

    inline void SET_REALM_NAME(const char* realm_name) {
        WotlkSDK::AutoLogin::SetRealmName(realm_name ? realm_name : "");
    }

    inline void SET_CHARACTER(const char* character) {
        WotlkSDK::AutoLogin::SetCharacter(character ? character : "");
    }

    inline std::string_view GET_LOGIN() {
        return WotlkSDK::AutoLogin::GetLogin();
    }

    inline std::string_view GET_PASSWORD() {
        return WotlkSDK::AutoLogin::GetPassword();
    }

    inline std::string_view GET_REALMLIST() {
        return WotlkSDK::AutoLogin::GetRealmlist();
    }

    inline std::string_view GET_REALM_NAME() {
        return WotlkSDK::AutoLogin::GetRealmName();
    }

    inline std::string_view GET_CHARACTER() {
        return WotlkSDK::AutoLogin::GetCharacter();
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
