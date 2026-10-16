#pragma once

#include <hookkit/transaction.h>
#include <string_view>

#include "Defines.h"

namespace AutoLogin {

/**
 * @brief Installs internal auto-login hooks into the provided hook transaction.
 * @note Typically called automatically by WotlkSDK::Initialize().
 * @param tx Active HookTransaction to register hooks with.
 */
WOTLK_SDK_API void Initialize(hookkit::HookTransaction& tx);
inline void initialize(hookkit::HookTransaction& tx) { Initialize(tx); }

/**
 * @brief Programmatically configures auto-login credentials and destination realm.
 *
 * Precedence hierarchy:
 * 1. Programmatic API (this method)
 * 2. Command-line flags (-login, -password, -realmlist, -realmname, -character)
 * 3. Environment variables (WOTLK_LOGIN, WOTLK_PASSWORD, etc.)
 * 4. Game defaults (realmlist.wtf and WTF/Config.wtf)
 *
 * @param login Account username.
 * @param password Account password.
 * @param realmlist Optional realmlist hostname/IP (overrides realmList CVar).
 * @param realmName Optional realm display name (overrides realmName CVar).
 * @param character Optional character name to automatically select and log into.
 */
WOTLK_SDK_API void Configure(std::string_view login, std::string_view password, std::string_view realmlist = {},
    std::string_view realmName = {}, std::string_view character = {});

WOTLK_SDK_API void SetLogin(std::string_view login);
WOTLK_SDK_API void SetPassword(std::string_view password);
WOTLK_SDK_API void SetRealmlist(std::string_view realmlist);
WOTLK_SDK_API void SetRealmName(std::string_view realmName);
WOTLK_SDK_API void SetCharacter(std::string_view character);

WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetLogin();
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetPassword();
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetRealmlist();
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetRealmName();
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetCharacter();

}  // namespace AutoLogin

namespace auto_login = AutoLogin;
namespace command_line = AutoLogin;
