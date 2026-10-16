#pragma once

/**
 * @file AutoLogin.h
 * @brief Automated client authentication, realm selection, and character login subsystem.
 *
 * Provides programmatic and command-line interfaces for bypassing manual credential entry,
 * configuring realmlist destinations, and auto-entering the world with a chosen character.
 */

#include <hookkit/transaction.h>
#include <string_view>

#include "Defines.h"

namespace AutoLogin {

/**
 * @brief Registers internal auto-login engine hooks within the provided hook transaction.
 * @note Invoked automatically by WotlkSDK::Initialize().
 * @param tx Active hook transaction handle.
 */
WOTLK_SDK_API void Initialize(hookkit::HookTransaction& tx);

/**
 * @brief Backward-compatible alias for Initialize().
 * @param tx Active hook transaction handle.
 */
inline void initialize(hookkit::HookTransaction& tx) { Initialize(tx); }

/**
 * @brief Programmatically configures auto-login credentials and realm destination parameters.
 *
 * Configuration resolution precedence:
 * 1. Programmatic API (this function)
 * 2. Command-line flags (-login/-user, -password/-pass, -realmlist/-server, -realmname/-realm, -character/-char)
 * 3. Environment variables (WOTLK_LOGIN, WOTLK_PASSWORD, WOTLK_REALMLIST, WOTLK_REALMNAME, WOTLK_CHARACTER)
 * 4. Client configuration files (Data/<locale>/realmlist.wtf and WTF/Config.wtf)
 *
 * @param login Account username.
 * @param password Account password.
 * @param realmlist Optional realmlist host address (overrides realmList CVar).
 * @param realmName Optional destination realm display name (overrides realmName CVar).
 * @param character Optional character name to select upon character enumeration.
 */
WOTLK_SDK_API void Configure(std::string_view login, std::string_view password, std::string_view realmlist = {},
    std::string_view realmName = {}, std::string_view character = {});

/**
 * @brief Sets the account username.
 * @param login Username string view.
 */
WOTLK_SDK_API void SetLogin(std::string_view login);

/**
 * @brief Sets the account password.
 * @param password Password string view.
 */
WOTLK_SDK_API void SetPassword(std::string_view password);

/**
 * @brief Sets the realmlist host address.
 * @param realmlist Hostname or IP string view.
 */
WOTLK_SDK_API void SetRealmlist(std::string_view realmlist);

/**
 * @brief Sets the target realm name.
 * @param realmName Realm display name string view.
 */
WOTLK_SDK_API void SetRealmName(std::string_view realmName);

/**
 * @brief Sets the target character name for automatic world entry.
 * @param character Character name string view.
 */
WOTLK_SDK_API void SetCharacter(std::string_view character);

/**
 * @brief Retrieves the currently configured account username.
 * @return std::string_view Configured username.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetLogin();

/**
 * @brief Retrieves the currently configured account password.
 * @return std::string_view Configured password.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetPassword();

/**
 * @brief Retrieves the currently configured realmlist host address.
 * @return std::string_view Configured realmlist host.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetRealmlist();

/**
 * @brief Retrieves the currently configured destination realm name.
 * @return std::string_view Configured realm name.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetRealmName();

/**
 * @brief Retrieves the currently configured character name.
 * @return std::string_view Configured character name.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API std::string_view GetCharacter();

}  // namespace AutoLogin

namespace auto_login = AutoLogin;
namespace command_line = AutoLogin;
