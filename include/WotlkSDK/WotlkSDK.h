#pragma once

#include <cstdint>
#include <string_view>

// WoW 3.3.5a Engine Headers
#include "BaseTypes.h"
#include "Constants.h"
#include "Client/Client.h"
#include "CVar/CVar.h"
#include "DB/ClientDB.h"
#include "DB/DBRecords.h"
#include "FrameScript/FrameScript.h"
#include "Game/CGGameUI.h"
#include "Game/CGInputControl.h"
#include "Glue/CGlueMgr.h"
#include "Glue/LoginUI.h"
#include "Graphics/BLP.h"
#include "Graphics/CGxDevice.h"
#include "Graphics/CGxFont.h"
#include "Graphics/CGxShader.h"
#include "Graphics/CGxString.h"
#include "Graphics/GraphicsEnums.h"
#include "Graphics/GxuFontUtil.h"
#include "Lib/FreeType.h"
#include "Lib/Lua.h"
#include "Lib/Storm.h"
#include "Net/NetClient.h"
#include "ObjectManager/CGObject_C.h"
#include "ObjectManager/CGPlayer_C.h"
#include "ObjectManager/CGUnit_C.h"
#include "ObjectManager/ObjectManager.h"
#include "System/System.h"

// WotlkSDK Extensions & Features
#include "WotlkSDK/AutoLogin.h"
#include "WotlkSDK/Extensions.h"

namespace WotlkSDK {

// SDK Version Information
inline constexpr uint32_t VERSION_MAJOR = 1;
inline constexpr uint32_t VERSION_MINOR = 0;
inline constexpr uint32_t VERSION_PATCH = 0;
inline constexpr const char* VERSION_STRING = "1.0.0";
inline constexpr uint32_t STARTUP_RETRY_BUDGET_MS = 5000;

// Main Lifecycle API
bool Initialize(uint32_t retryBudgetMs = STARTUP_RETRY_BUDGET_MS);
bool IsInitialized();

// Namespaces in UpperCamelCase
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
