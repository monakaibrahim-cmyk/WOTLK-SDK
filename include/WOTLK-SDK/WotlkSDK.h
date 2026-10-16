#pragma once

#include <cstdint>
#include <string_view>

// SDK Core Definitions & Versioning
#include "Defines.h"
#include "Version.h"

// WoW 3.3.5a Engine Reverse-Engineered Subsystems
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

// WotlkSDK Extensions & Features
#include "AutoLogin.h"
#include "Extensions.h"
#include "API.h"
#include "Discord.h"

namespace WotlkSDK {

inline constexpr uint32_t STARTUP_RETRY_BUDGET_MS = 5000;

/**
 * @brief Initializes the WotlkSDK, patches required WoW engine pointers,
 *        and installs all core hook transactions.
 *
 * @param retryBudgetMs Maximum duration (in milliseconds) hook transactions will attempt
 *                      atomic installation before aborting. Default is 5000 ms.
 * @return true if all hooks were successfully installed; false otherwise.
 */
WOTLK_SDK_API bool Initialize(uint32_t retryBudgetMs = STARTUP_RETRY_BUDGET_MS);

/**
 * @brief Checks if WotlkSDK has already been initialized in the current process.
 * @return true if initialized, false otherwise.
 */
WOTLK_SDK_NODISCARD WOTLK_SDK_API bool IsInitialized();

/**
 * @brief Shuts down background workers (Discord Rich Presence) and releases SDK resources.
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
inline constexpr auto& CVAR_REGISTRY = ::Extensions::Console::CVAR_REGISTRY;
inline constexpr auto& EVENT_REGISTRY = ::Extensions::Console::EVENT_REGISTRY;
inline constexpr auto& LUA_LIB_REGISTRY = ::Extensions::Console::LUA_LIB_REGISTRY;
inline constexpr auto& TOKEN_REGISTRY = ::Extensions::FrameScript::TOKEN_REGISTRY;
inline constexpr auto& ON_UPDATE = ::Extensions::FrameScript::ON_UPDATE;
inline constexpr auto& ON_ENTER = ::Extensions::FrameScript::ON_ENTER;
inline constexpr auto& ON_LEAVE = ::Extensions::FrameScript::ON_LEAVE;
inline constexpr auto& POST_LOAD = ::Extensions::Glue::POST_LOAD;
inline constexpr auto& CHAR_ENUM = ::Extensions::Glue::CHAR_ENUM;

}  // namespace WotlkSDK
