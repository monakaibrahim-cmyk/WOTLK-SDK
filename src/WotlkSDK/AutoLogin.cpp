#include "WotlkSDK/AutoLogin.h"

#include <shellapi.h>

#include <algorithm>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "CVar/CVar.h"
#include "Glue/LoginUI.h"
#include "Net/NetClient.h"
#include "Utils.h"
#include "WotlkSDK/Extensions.h"

namespace {
inline constexpr utils::Accessor<std::vector<std::string>, struct ArgsTag> ARGS;

std::string g_configuredLogin;
std::string g_configuredPassword;
std::string g_configuredRealmlist;
std::string g_configuredRealmName;
std::string g_configuredCharacter;

std::string_view flagName(std::string_view arg) {
    if (!arg.starts_with('-') && !arg.starts_with('/')) { return {}; }
    arg.remove_prefix(1);
    if (arg.starts_with('-')) { arg.remove_prefix(1); }
    return arg;
}

const std::string* getParam(std::string_view item, const char* env_var = nullptr) {
    // 1. Programmatic configuration takes highest priority
    if (item == "login" && !g_configuredLogin.empty()) { return &g_configuredLogin; }
    if (item == "password" && !g_configuredPassword.empty()) { return &g_configuredPassword; }
    if (item == "realmlist" && !g_configuredRealmlist.empty()) { return &g_configuredRealmlist; }
    if (item == "realmname" && !g_configuredRealmName.empty()) { return &g_configuredRealmName; }
    if (item == "character" && !g_configuredCharacter.empty()) { return &g_configuredCharacter; }

    // 2. Command line arguments (-login, -password, etc.)
    if (!item.empty() && !ARGS->empty()) {
        const std::span tail = std::span(*ARGS).subspan(1);
        const auto key = std::ranges::adjacent_find(
            tail, [item](const std::string& arg, const std::string&) { return flagName(arg) == item; });
        if (key != tail.end()) { return &*std::next(key); }
    }

    // 3. Environment variables (WOTLK_LOGIN, WOTLK_PASSWORD, etc.)
    if (env_var != nullptr) {
        static std::vector<std::pair<std::string, std::string>> env_cache;
        for (const auto& [name, val] : env_cache) {
            if (name == env_var) { return &val; }
        }
        char buf[512] = {};
        const DWORD len = GetEnvironmentVariableA(env_var, buf, sizeof(buf));
        if (len > 0 && len < sizeof(buf)) {
            env_cache.emplace_back(env_var, std::string(buf, len));
            return &env_cache.back().second;
        }
    }
    return nullptr;
}

void setCVarFromParam(std::string_view param_name, const char* cvar_name, const char* env_var = nullptr) {
    const std::string* val = getParam(param_name, env_var);
    if (val == nullptr) { return; }
    if (CVar* cvar = CVar::find{}(cvar_name)) { cvar->setValue(val->c_str(), 1, 0, 0, 1); }
}

void onCharEnum() {
    static bool once = false;
    if (once) { return; }

    const std::string* character = getParam("character", "WOTLK_CHARACTER");
    if (character == nullptr) { return; }

    Login::CharacterSelectionDisplay::TSGRA.enumerate([&](const Login::CharacterSelectionDisplay& chr, uint32_t idx) {
        if (*character != std::string_view(std::data(chr.name))) { return true; }
        once = true;
        Login::enterWorld(static_cast<int>(idx));
        return false;
    });
}

void onPostLoad() {
    static bool once = false;
    if (once) { return; }
    once = true;

    setCVarFromParam("realmlist", "realmList", "WOTLK_REALMLIST");
    setCVarFromParam("realmname", "realmName", "WOTLK_REALMNAME");

    const std::string* user = getParam("login", "WOTLK_LOGIN");
    const std::string* pass = getParam("password", "WOTLK_PASSWORD");
    if (user != nullptr && pass != nullptr) { NetClient::login{}(user->c_str(), pass->c_str()); }
}
}  // namespace

namespace AutoLogin {

void Initialize(hookkit::HookTransaction&) {
    int argc = 0;
    if (wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc)) {
        ARGS->reserve(static_cast<size_t>(argc));
        std::ranges::transform(
            std::span(argv, static_cast<size_t>(argc)), std::back_inserter(*ARGS), Utils::wideToUtf8);
        LocalFree(argv);
    }

    Extensions::Glue::CHAR_ENUM->add(onCharEnum);
    Extensions::Glue::POST_LOAD->add(onPostLoad);
}

void Configure(std::string_view login, std::string_view password, std::string_view realmlist,
    std::string_view realmName, std::string_view character) {
    SetLogin(login);
    SetPassword(password);
    if (!realmlist.empty()) { SetRealmlist(realmlist); }
    if (!realmName.empty()) { SetRealmName(realmName); }
    if (!character.empty()) { SetCharacter(character); }
}

void SetLogin(std::string_view login) { g_configuredLogin = login; }
void SetPassword(std::string_view password) { g_configuredPassword = password; }
void SetRealmlist(std::string_view realmlist) { g_configuredRealmlist = realmlist; }
void SetRealmName(std::string_view realmName) { g_configuredRealmName = realmName; }
void SetCharacter(std::string_view character) { g_configuredCharacter = character; }

std::string_view GetLogin() {
    const std::string* val = getParam("login", "WOTLK_LOGIN");
    return val != nullptr ? *val : "";
}

std::string_view GetPassword() {
    const std::string* val = getParam("password", "WOTLK_PASSWORD");
    return val != nullptr ? *val : "";
}

std::string_view GetRealmlist() {
    const std::string* val = getParam("realmlist", "WOTLK_REALMLIST");
    return val != nullptr ? *val : "";
}

std::string_view GetRealmName() {
    const std::string* val = getParam("realmname", "WOTLK_REALMNAME");
    return val != nullptr ? *val : "";
}

std::string_view GetCharacter() {
    const std::string* val = getParam("character", "WOTLK_CHARACTER");
    return val != nullptr ? *val : "";
}

}  // namespace AutoLogin
