#pragma once

/**
 * @file hookkit.h
 * @brief Primary include header for HookKit / HOOK-KIT framework.
 *
 * Consolidates detours, trampolines, transaction management, calling convention descriptors,
 * site hooks, and wild register ABI manipulation into the HookKit namespace.
 */

#include "abi_macros.h"
#include "accessor.h"
#include "desc.h"
#include "hook.h"
#include "patch.h"
#include "sitehook.h"
#include "trampoline.h"
#include "transaction.h"
#include "wildabi.h"

namespace HookKit {
using ::HookKit::Conv;
using ::HookKit::Hook;
using ::HookKit::Desc;
using ::HookKit::HookTransaction;
using ::HookKit::RetryBudget;
using ::HookKit::ScopedUnprotect;
using ::HookKit::PatchBytes;
using ::HookKit::ForceWrite;
using ::HookKit::Targets;
using ::HookKit::Branch;
using ::HookKit::BranchMap;
using ::HookKit::SiteHook;
using ::HookKit::CallsiteHook;
using ::HookKit::WildAbi;
using ::HookKit::WildHook;
using ::HookKit::ArgLoc;
using ::HookKit::Reg;
}  // namespace HookKit

namespace hookkit = HookKit;
namespace HOOK_KIT = HookKit;
namespace HOOKKIT = HookKit;
