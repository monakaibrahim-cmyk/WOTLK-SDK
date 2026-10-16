#pragma once

/**
 * @file WotlkSDK.h
 * @brief Main C++ header for WotlkSDK (World of Warcraft 3.3.5a build 12340).
 *
 * Consolidates reverse-engineered WoW client engine subsystems, extensions,
 * auto-login, Discord Rich Presence, and atomic hooking tools into a unified interface.
 */

#include <cstdint>
#include <string_view>

// Core Definitions & Versioning
#include "Defines.h"
#include "Version.h"

// Engine Reverse-Engineered Subsystems
#include "../BaseTypes.h"
#include "../Constants.h"
#include "../Client/Client.h"
#include "../CVar/CVar.h"
#include "../DB/ClientDB.h"
#include "../DB/DBRecords.h"
#include "../FrameScript/FrameScript.h"
#include "../Game/CGGameUI.h"
#include "../Game/CGInputControl.h"
#include "../Glue/CGlueMgr.h"
#include "../Glue/LoginUI.h"
#include "../Graphics/BLP.h"
#include "../Graphics/CGxDevice.h"
#include "../Graphics/CGxFont.h"
#include "../Graphics/CGxShader.h"
#include "../Graphics/CGxString.h"
#include "../Graphics/GraphicsEnums.h"
#include "../Graphics/GxuFontUtil.h"
#include "../Lib/FreeType.h"
#include "../Lib/Lua.h"
#include "../Lib/Storm.h"
#include "../Net/NetClient.h"
#include "../ObjectManager/CGObject_C.h"
#include "../ObjectManager/CGPlayer_C.h"
#include "../ObjectManager/CGUnit_C.h"
#include "../ObjectManager/ObjectManager.h"
#include "../System/System.h"

// SDK Modules & Registries
#include "AutoLogin.h"
#include "Extensions.h"
#include "API.h"
#include "Discord.h"

namespace WotlkSDK {

/**
 * @brief Default hook installation retry timeout in milliseconds.
 */
inline constexpr uint32_t STARTUP_RETRY_BUDGET_MS = 5000;

/**
 * @brief Initializes WotlkSDK, patches client engine validation pointers,
 *        and atomically installs core startup hook transactions.
 *
 * @param retryBudgetMs Maximum duration in milliseconds to retry atomic hook installation.
 * @return true if all hooks installed successfully, false otherwise.
 */
WOTLK_SDK_API bool Initialize(uint32_t retryBudgetMs = STARTUP_RETRY_BUDGET_MS);

/**
 * @brief Checks if WotlkSDK has already initialized in the current process.
 * @return true if initialized, false otherwise.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API bool IsInitialized();

/**
 * @brief Gracefully terminates background worker threads (Discord Rich Presence) and releases SDK resources.
 */
WOTLK_SDK_API void Shutdown();

// Submodules in UpperCamelCase
namespace Discord = ::WotlkSDK::Discord;
namespace DISCORD = ::WotlkSDK::DISCORD;
namespace AutoLogin = ::AutoLogin;
namespace Extensions = ::Extensions;
namespace Console = ::Extensions::Console;
namespace FrameScript = ::Extensions::FrameScript;
namespace Glue = ::Extensions::Glue;
namespace ObjectMgr = ::ObjectMgr;
namespace Storm = ::Storm;
namespace Lua = ::Lua;
namespace NetClient = ::NetClient;
namespace GameUI = ::GameUI;
namespace Login = ::Login;
namespace ClientDB = ::ClientDB;
namespace Client = ::Client;
namespace OS = ::OS;

// Direct SCREAMING_SNAKE_CASE Accessors for Developer Convenience

/** @brief Global CVar registry for registering custom console variables. */
inline constexpr auto& CVAR_REGISTRY = ::Extensions::Console::CVAR_REGISTRY;

/** @brief Event registry for registering and firing FrameScript events. */
inline constexpr auto& EVENT_REGISTRY = ::Extensions::Console::EVENT_REGISTRY;

/** @brief Lua library registry for exposing custom C/C++ functions to Lua. */
inline constexpr auto& LUA_LIB_REGISTRY = ::Extensions::Console::LUA_LIB_REGISTRY;

/** @brief Token registry for custom unit token resolution. */
inline constexpr auto& TOKEN_REGISTRY = ::Extensions::FrameScript::TOKEN_REGISTRY;

/** @brief Per-frame render tick callback list. */
inline constexpr auto& ON_UPDATE = ::Extensions::FrameScript::ON_UPDATE;

/** @brief World enter event callback list (fired when player enters world). */
inline constexpr auto& ON_ENTER = ::Extensions::FrameScript::ON_ENTER;

/** @brief World leave event callback list (fired when player leaves world). */
inline constexpr auto& ON_LEAVE = ::Extensions::FrameScript::ON_LEAVE;

/** @brief Post-load callback list (fired after glue XML interface loads). */
inline constexpr auto& POST_LOAD = ::Extensions::Glue::POST_LOAD;

/** @brief Character enumeration callback list (fired when character list populates). */
inline constexpr auto& CHAR_ENUM = ::Extensions::Glue::CHAR_ENUM;

}  // namespace WotlkSDK
