#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace Client {
inline const std::span<const char* const, 9> LOCALES{reinterpret_cast<const char* const*>(0x00AD2FE0), 9};
inline int32_t& CURRENT_LOCALE_ID = *reinterpret_cast<int32_t*>(0x00C5DE9C);

inline const auto& kLocales = LOCALES;
inline int32_t& current_locale_id = CURRENT_LOCALE_ID;

inline std::string getGameLocale() { return LOCALES[CURRENT_LOCALE_ID]; }
}  // namespace Client
namespace client = Client;
