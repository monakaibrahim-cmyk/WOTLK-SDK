/**
 * @file Entry.cpp
 * @brief WotlkSDK primary lifecycle initialization, memory patching, and DLL entry point.
 */

#include <windows.h>

#include <cassert>

#include "Lib/Lua.h"
#include "WOTLK-SDK/AutoLogin.h"
#include "WOTLK-SDK/Extensions.h"
#include "WOTLK-SDK/WotlkSDK.h"

namespace {
bool g_initialized = false;

#ifdef _DEBUG
/**
 * @brief Lua debug breakpoint hook for developer inspection in debug configurations.
 */
int luaDebugBreak(LuaState*) {
    if (IsDebuggerPresent()) { DebugBreak(); }
    return 0;
}
#endif

/**
 * @brief Registers SDK global flags and diagnostics into the client Lua 5.1 runtime.
 */
int luaOpenWotlkSDK(LuaState* l) {
    Lua::pushNumber(l, 1);
    Lua::setGlobal(l, "WOTLK_SDK");
    Lua::pushNumber(l, 1);
    Lua::setGlobal(l, "WotlkSDK");

#ifdef _DEBUG
    Lua::pushCFunction(l, luaDebugBreak);
    Lua::setGlobal(l, "DEBUGBREAK");
#endif
    return 0;
}
}  // namespace

namespace WotlkSDK {

bool Initialize(uint32_t retryBudgetMs) {
    if (g_initialized) { return true; }

    // Bypass engine function pointer validation checks (0x00D415B8, 0x00D415BC)
    *reinterpret_cast<DWORD*>(0x00D415B8) = 1;
    *reinterpret_cast<DWORD*>(0x00D415BC) = 0x7FFFFFFF;

    // Automatically accept startup legal dialogs (TOS and EULA)
    *reinterpret_cast<DWORD*>(0x00B6AF54) = 1;
    *reinterpret_cast<DWORD*>(0x00B6AF5C) = 1;

    // Atomically install extension and auto-login hook transactions
    hookkit::HookTransaction tx{hookkit::RetryBudget{retryBudgetMs}};

    Extensions::Initialize(tx);
    AutoLogin::Initialize(tx);

    if (tx.commit() != NO_ERROR) {
        assert(false && "WotlkSDK: startup hooks failed to install");
        return false;
    }

    // Register SDK Lua globals
    Extensions::Console::LUA_LIB_REGISTRY->add(luaOpenWotlkSDK);
    g_initialized = true;

#ifdef DISCORD_RICH_PRESENCE
    if (!std::string_view(DISCORD_CLIENT_ID).empty()) {
        Discord::Start(DISCORD_CLIENT_ID);
    }
#endif

    return true;
}

bool IsInitialized() {
    return g_initialized;
}

void Shutdown() {
    if (!g_initialized) { return; }
    Discord::Stop();
    g_initialized = false;
}

}  // namespace WotlkSDK

#if defined(WOTLK_SDK_BUILD_SHARED) || defined(WOTLK_SDK_SHARED)
/**
 * @brief Dynamic link library entry point, active only when compiling WotlkSDK as a shared DLL.
 */
BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(module);
        WotlkSDK::Initialize();
    } else if (reason == DLL_PROCESS_DETACH) {
        WotlkSDK::Shutdown();
    }
    return TRUE;
}
#endif
