#include <windows.h>

#include <cassert>

#include "Lib/Lua.h"
#include "WOTLK-SDK/AutoLogin.h"
#include "WOTLK-SDK/Extensions.h"
#include "WOTLK-SDK/WotlkSDK.h"

namespace {
bool g_initialized = false;

#ifdef _DEBUG
int luaDebugBreak(LuaState*) {
    if (IsDebuggerPresent()) { DebugBreak(); }
    return 0;
}
#endif

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

    // Invalid function pointer hack
    *reinterpret_cast<DWORD*>(0x00D415B8) = 1;
    *reinterpret_cast<DWORD*>(0x00D415BC) = 0x7FFFFFFF;

    *reinterpret_cast<DWORD*>(0x00B6AF54) = 1;  // TOSAccepted
    *reinterpret_cast<DWORD*>(0x00B6AF5C) = 1;  // EULAAccepted

    hookkit::HookTransaction tx{hookkit::RetryBudget{retryBudgetMs}};

    Extensions::Initialize(tx);
    AutoLogin::Initialize(tx);

    if (tx.commit() != NO_ERROR) {
        assert(false && "WotlkSDK: startup hooks failed to install");
        return false;
    }

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

// Standalone DLL entry point - only enabled when building as a shared DLL
#if defined(WOTLK_SDK_BUILD_SHARED) || defined(WOTLK_SDK_SHARED)
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
