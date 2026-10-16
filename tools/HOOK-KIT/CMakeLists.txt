add_subdirectory(MinHook)

set(ASMJIT_STATIC ON)
add_subdirectory(asmjit)

add_library(Hookkit INTERFACE)
target_include_directories(Hookkit INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
target_link_libraries(Hookkit INTERFACE MinHook asmjit)

add_library(HookKit::HookKit ALIAS Hookkit)
add_library(HOOK_KIT::HOOK_KIT ALIAS Hookkit)
add_library(HOOKKIT::HOOKKIT ALIAS Hookkit)
