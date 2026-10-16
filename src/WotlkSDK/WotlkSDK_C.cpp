#include "WotlkSDK/WotlkSDK_C.h"
#include "WotlkSDK/WotlkSDK.h"

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
