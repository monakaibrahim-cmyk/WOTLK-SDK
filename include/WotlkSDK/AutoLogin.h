#pragma once

#include <hookkit/transaction.h>
#include <string_view>

namespace AutoLogin {

// Lifecycle hook installation (called internally during WotlkSDK::Initialize)
void Initialize(hookkit::HookTransaction& tx);
inline void initialize(hookkit::HookTransaction& tx) { Initialize(tx); }

// Programmatic configuration API
void Configure(std::string_view login, std::string_view password, std::string_view realmlist = {},
    std::string_view realmName = {}, std::string_view character = {});

void SetLogin(std::string_view login);
void SetPassword(std::string_view password);
void SetRealmlist(std::string_view realmlist);
void SetRealmName(std::string_view realmName);
void SetCharacter(std::string_view character);

[[nodiscard]] std::string_view GetLogin();
[[nodiscard]] std::string_view GetPassword();
[[nodiscard]] std::string_view GetRealmlist();
[[nodiscard]] std::string_view GetRealmName();
[[nodiscard]] std::string_view GetCharacter();

}  // namespace AutoLogin

namespace auto_login = AutoLogin;
namespace command_line = AutoLogin;
