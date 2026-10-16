#pragma once

#include <HOOK-KIT/hook.h>

namespace NetClient {
// login, passwd
HOOKKIT_HOOK_HANDLE(login, 0x004D8A30, hookkit::Conv::eCdecl, void, const char*, const char*);
}  // namespace NetClient
namespace net_client = NetClient;
