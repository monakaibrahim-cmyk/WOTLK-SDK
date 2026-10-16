#include "WOTLK-SDK/WotlkSDK_C.h"
#include "WOTLK-SDK/WotlkSDK.h"

// Version
const char* wotlk_sdk_get_version_string(void) {
    return WotlkSDK::VERSION_STRING;
}

uint32_t wotlk_sdk_get_version_major(void) {
    return WotlkSDK::VERSION_MAJOR;
}

uint32_t wotlk_sdk_get_version_minor(void) {
    return WotlkSDK::VERSION_MINOR;
}

uint32_t wotlk_sdk_get_version_patch(void) {
    return WotlkSDK::VERSION_PATCH;
}

uint32_t wotlk_sdk_get_target_wow_build(void) {
    return WotlkSDK::TARGET_WOW_BUILD;
}

// Lifecycle
int wotlk_sdk_initialize(uint32_t retry_budget_ms) {
    return WotlkSDK::Initialize(retry_budget_ms) ? 1 : 0;
}

int wotlk_sdk_is_initialized(void) {
    return WotlkSDK::IsInitialized() ? 1 : 0;
}

void wotlk_sdk_shutdown(void) {
    WotlkSDK::Shutdown();
}

// AutoLogin
void wotlk_autologin_configure(
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

void wotlk_autologin_set_login(const char* login) {
    WotlkSDK::AutoLogin::SetLogin(login ? login : "");
}

void wotlk_autologin_set_password(const char* password) {
    WotlkSDK::AutoLogin::SetPassword(password ? password : "");
}

void wotlk_autologin_set_realmlist(const char* realmlist) {
    WotlkSDK::AutoLogin::SetRealmlist(realmlist ? realmlist : "");
}

void wotlk_autologin_set_realm_name(const char* realm_name) {
    WotlkSDK::AutoLogin::SetRealmName(realm_name ? realm_name : "");
}

void wotlk_autologin_set_character(const char* character) {
    WotlkSDK::AutoLogin::SetCharacter(character ? character : "");
}

const char* wotlk_autologin_get_login(void) {
    return WotlkSDK::AutoLogin::GetLogin().data();
}

const char* wotlk_autologin_get_password(void) {
    return WotlkSDK::AutoLogin::GetPassword().data();
}

const char* wotlk_autologin_get_realmlist(void) {
    return WotlkSDK::AutoLogin::GetRealmlist().data();
}

const char* wotlk_autologin_get_realm_name(void) {
    return WotlkSDK::AutoLogin::GetRealmName().data();
}

const char* wotlk_autologin_get_character(void) {
    return WotlkSDK::AutoLogin::GetCharacter().data();
}

// Callbacks
void wotlk_on_update_add(wotlk_callback_fn callback) {
    if (callback) {
        WotlkSDK::ON_UPDATE->add([callback]() { callback(); });
    }
}

void wotlk_on_enter_add(wotlk_callback_fn callback) {
    if (callback) {
        WotlkSDK::ON_ENTER->add([callback]() { callback(); });
    }
}

void wotlk_on_leave_add(wotlk_callback_fn callback) {
    if (callback) {
        WotlkSDK::ON_LEAVE->add([callback]() { callback(); });
    }
}

void wotlk_on_post_load_add(wotlk_callback_fn callback) {
    if (callback) {
        WotlkSDK::POST_LOAD->add([callback]() { callback(); });
    }
}

void wotlk_on_char_enum_add(wotlk_callback_fn callback) {
    if (callback) {
        WotlkSDK::CHAR_ENUM->add([callback]() { callback(); });
    }
}

// CVars
void wotlk_cvar_register_int(
    const char* name,
    const char* desc,
    int init_val,
    int min_val,
    int max_val,
    wotlk_cvar_change_int_fn on_change
) {
    WotlkSDK::CVAR_REGISTRY->add<int>({
        .name = name,
        .desc = desc,
        .init = init_val,
        .min = Extensions::Console::Bound<int>(min_val),
        .max = Extensions::Console::Bound<int>(max_val),
        .on_change = [on_change](int val, bool changed) {
            if (on_change) { on_change(val, changed ? 1 : 0); }
        }
    });
}

void wotlk_cvar_register_float(
    const char* name,
    const char* desc,
    float init_val,
    float min_val,
    float max_val,
    wotlk_cvar_change_float_fn on_change
) {
    WotlkSDK::CVAR_REGISTRY->add<float>({
        .name = name,
        .desc = desc,
        .init = init_val,
        .min = Extensions::Console::Bound<float>(min_val),
        .max = Extensions::Console::Bound<float>(max_val),
        .on_change = [on_change](float val, bool changed) {
            if (on_change) { on_change(val, changed ? 1 : 0); }
        }
    });
}

int wotlk_cvar_get_int(const char* name) {
    return WotlkSDK::CVAR_REGISTRY->get<int>(name ? name : "");
}

float wotlk_cvar_get_float(const char* name) {
    return WotlkSDK::CVAR_REGISTRY->get<float>(name ? name : "");
}

void wotlk_cvar_set(const char* name, const char* value) {
    WotlkSDK::CVAR_REGISTRY->set(name ? name : "", value ? value : "");
}

void wotlk_cvar_refresh(const char* name) {
    WotlkSDK::CVAR_REGISTRY->refresh(name ? name : "");
}

// Queries
wotlk_guid_t wotlk_get_player_guid(void) {
    return ObjectMgr::getPlayerGuid();
}

wotlk_guid_t wotlk_get_target_guid(void) {
    return ObjectMgr::getTargetGuid();
}

int wotlk_is_in_world(void) {
    return GameUI::isInWorld() ? 1 : 0;
}

int wotlk_in_combat_lockdown(void) {
    return GameUI::inCombatLockdown() ? 1 : 0;
}

void* wotlk_get_game_window(void) {
    return reinterpret_cast<void*>(OS::getGameWindow());
}

uint64_t wotlk_get_async_time_ms(void) {
    return OS::getAsyncTimeMs{}();
}

/* ========================================================================= */
/* Typed Pure C API Implementations                                          */
/* ========================================================================= */

// PLAYER
wotlk_guid_t PLAYER_GET_UID(void) {
    return WotlkSDK::PLAYER::GET_UID();
}
wotlk_guid_t PLAYER_GET_GUID(void) {
    return WotlkSDK::PLAYER::GET_GUID();
}
wotlk_guid_t PLAYER_GET_TARGET_UID(void) {
    return WotlkSDK::PLAYER::GET_TARGET_UID();
}
wotlk_guid_t PLAYER_GET_TARGET_GUID(void) {
    return WotlkSDK::PLAYER::GET_TARGET_GUID();
}
const char* PLAYER_GET_NAME(void) {
    return WotlkSDK::PLAYER::GET_NAME();
}
uint32_t PLAYER_GET_LEVEL(void) {
    return WotlkSDK::PLAYER::GET_LEVEL();
}
uint8_t PLAYER_GET_RACE(void) {
    return WotlkSDK::PLAYER::GET_RACE();
}
const char* PLAYER_GET_RACE_NAME(void) {
    return WotlkSDK::PLAYER::GET_RACE_NAME();
}
uint8_t PLAYER_GET_CLASS(void) {
    return WotlkSDK::PLAYER::GET_CLASS();
}
const char* PLAYER_GET_CLASS_NAME(void) {
    return WotlkSDK::PLAYER::GET_CLASS_NAME();
}
uint8_t PLAYER_GET_GENDER(void) {
    return WotlkSDK::PLAYER::GET_GENDER();
}
const char* PLAYER_GET_GENDER_NAME(void) {
    return WotlkSDK::PLAYER::GET_GENDER_NAME();
}
uint32_t PLAYER_GET_HEALTH(void) {
    return WotlkSDK::PLAYER::GET_HEALTH();
}
uint32_t PLAYER_GET_MAX_HEALTH(void) {
    return WotlkSDK::PLAYER::GET_MAX_HEALTH();
}
uint32_t PLAYER_GET_POWER(uint32_t index) {
    return WotlkSDK::PLAYER::GET_POWER(index);
}
uint32_t PLAYER_GET_MAX_POWER(uint32_t index) {
    return WotlkSDK::PLAYER::GET_MAX_POWER(index);
}
uint32_t PLAYER_GET_MANA(void) {
    return WotlkSDK::PLAYER::GET_MANA();
}
uint32_t PLAYER_GET_MAX_MANA(void) {
    return WotlkSDK::PLAYER::GET_MAX_MANA();
}
void PLAYER_GET_POSITION(float* out_x, float* out_y, float* out_z) {
    Vec3f pos = WotlkSDK::PLAYER::GET_POSITION();
    if (out_x) *out_x = pos.x;
    if (out_y) *out_y = pos.y;
    if (out_z) *out_z = pos.z;
}
float PLAYER_GET_FACING(void) {
    return WotlkSDK::PLAYER::GET_FACING();
}
int PLAYER_IS_IN_COMBAT(void) {
    return WotlkSDK::PLAYER::IS_IN_COMBAT() ? 1 : 0;
}
int PLAYER_IS_ALIVE(void) {
    return WotlkSDK::PLAYER::IS_ALIVE() ? 1 : 0;
}
int PLAYER_IS_DEAD(void) {
    return WotlkSDK::PLAYER::IS_DEAD() ? 1 : 0;
}
int PLAYER_IS_GHOST(void) {
    return WotlkSDK::PLAYER::IS_GHOST() ? 1 : 0;
}
void PLAYER_SET_TARGET(wotlk_guid_t guid) {
    WotlkSDK::PLAYER::SET_TARGET(guid);
}
void PLAYER_CLEAR_TARGET(void) {
    WotlkSDK::PLAYER::CLEAR_TARGET();
}

// SPELL
uint32_t SPELL_GET_ID(void) {
    return WotlkSDK::SPELL::GET_ID();
}
int SPELL_IS_TARGETING_AOE(void) {
    return WotlkSDK::SPELL::IS_TARGETING_AOE() ? 1 : 0;
}
int SPELL_CANCEL_AOE_TARGETING(void) {
    return WotlkSDK::SPELL::CANCEL_AOE_TARGETING();
}
int SPELL_CAST(uint32_t spell_id, wotlk_guid_t target) {
    return WotlkSDK::SPELL::CAST(spell_id, nullptr, target);
}

// GAME
uint32_t GAME_GET_STATE(void) {
    return WotlkSDK::GAME::GET_STATE();
}
const char* GAME_GET_STATE_NAME(void) {
    return WotlkSDK::GAME::GET_STATE_NAME();
}
int GAME_IS_IN_WORLD(void) {
    return WotlkSDK::GAME::IS_IN_WORLD() ? 1 : 0;
}
int GAME_IS_LOADING(void) {
    return WotlkSDK::GAME::IS_LOADING() ? 1 : 0;
}
void* GAME_GET_WINDOW(void) {
    return WotlkSDK::GAME::GET_WINDOW();
}
uint64_t GAME_GET_TIME_MS(void) {
    return WotlkSDK::GAME::GET_TIME_MS();
}

// ZONE
const char* ZONE_GET_TEXT(void) {
    return WotlkSDK::ZONE::GET_TEXT();
}
const char* ZONE_GET_ZONE_TEXT(void) {
    return WotlkSDK::ZONE::GET_ZONE_TEXT();
}
const char* ZONE_GET_SUBZONE_TEXT(void) {
    return WotlkSDK::ZONE::GET_SUBZONE_TEXT();
}
const char* ZONE_GET_MINIMAP_ZONE_TEXT(void) {
    return WotlkSDK::ZONE::GET_MINIMAP_ZONE_TEXT();
}
uint32_t ZONE_GET_AREA_ID(void) {
    return WotlkSDK::ZONE::GET_AREA_ID();
}
uint32_t ZONE_GET_MAP_ID(void) {
    return WotlkSDK::ZONE::GET_MAP_ID();
}
const char* ZONE_GET_MAP_NAME(void) {
    return WotlkSDK::ZONE::GET_MAP_NAME();
}
const char* ZONE_GET_CONTINENT_NAME(void) {
    return WotlkSDK::ZONE::GET_CONTINENT_NAME();
}

// PRESENCE
void PRESENCE_GET_DATA(wotlk_rich_presence_t* out_data) {
    if (!out_data) { return; }
    auto d = WotlkSDK::PRESENCE::GET_DATA();
    out_data->game_state = d.game_state;
    out_data->game_state_name = d.game_state_name;
    out_data->character_name = d.character_name;
    out_data->level = d.level;
    out_data->race_name = d.race_name;
    out_data->class_name = d.class_name;
    out_data->zone_text = d.zone_text;
    out_data->subzone_text = d.subzone_text;
    out_data->continent_name = d.continent_name;
    out_data->map_name = d.map_name;
    out_data->map_id = d.map_id;
    out_data->area_id = d.area_id;
    out_data->is_in_world = d.is_in_world ? 1 : 0;
    out_data->is_loading = d.is_loading ? 1 : 0;
    out_data->is_in_combat = d.is_in_combat ? 1 : 0;
}

// LUA
int LUA_EXECUTE(const char* script) {
    return WotlkSDK::LUA::EXECUTE(script);
}

// DISCORD
int wotlk_discord_start(const char* client_id, uint32_t interval_ms) {
    return WotlkSDK::Discord::Start(client_id ? client_id : DISCORD_CLIENT_ID, interval_ms) ? 1 : 0;
}

void wotlk_discord_stop(void) {
    WotlkSDK::Discord::Stop();
}

void wotlk_discord_update(void) {
    WotlkSDK::Discord::Update();
}

int wotlk_discord_is_connected(void) {
    return WotlkSDK::Discord::IsConnected() ? 1 : 0;
}

void* wotlk_discord_get_core(void) {
    return WotlkSDK::Discord::GetCore();
}
