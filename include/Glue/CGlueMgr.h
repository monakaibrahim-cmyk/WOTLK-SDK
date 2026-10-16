#pragma once

#include <HOOK-KIT/hook.h>

namespace Glue {
HOOKKIT_NAMED_HOOK(loadGlueXML_site, 0x004DA9A7, {"jmpback", 0x004DA9AC}, {"resumeStatusDtor", 0x004DA2D0});
HOOKKIT_HOOK_HANDLE(loadCharacters_site, 0x004E47E5, hookkit::Conv::eCdecl, void);
}  // namespace Glue
namespace glue = Glue;
