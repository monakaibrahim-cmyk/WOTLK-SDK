project(DiscordSDKExample LANGUAGES CXX)

# Discord Social SDK configuration
set(DISCORD_SDK_ROOT "${CMAKE_SOURCE_DIR}/lib/discord_social_sdk")
set(DISCORD_SDK_LIB_DIR "${DISCORD_SDK_ROOT}/lib/release")
set(DISCORD_SDK_BIN_DIR "${DISCORD_SDK_ROOT}/bin/release")
set(DISCORD_SDK_INCLUDE_DIR "${DISCORD_SDK_ROOT}/include")

add_library(DiscordSDKExample SHARED
    Main.cpp
)

target_link_libraries(DiscordSDKExample PRIVATE
    WotlkSDK::WotlkSDK
)

target_compile_definitions(DiscordSDKExample PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if (EXISTS "${DISCORD_SDK_INCLUDE_DIR}")
    target_include_directories(DiscordSDKExample PRIVATE ${DISCORD_SDK_INCLUDE_DIR})
endif()

# Platform-specific Social SDK library paths
if(WIN32)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/discord_partner_sdk.lib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_BIN_DIR}/discord_partner_sdk.dll")
elseif(APPLE)
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.dylib")
else() # Linux
    set(DISCORD_LIB_PATH "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
    set(DISCORD_SHARED_LIB "${DISCORD_SDK_LIB_DIR}/libdiscord_partner_sdk.so")
endif()

# Fallback search if fixed release path does not exist
if (NOT EXISTS "${DISCORD_LIB_PATH}")
    find_library(DISCORD_SDK_FALLBACK_LIB
        NAMES discord_partner_sdk discord_game_sdk discord_game_sdk.dll.lib
        PATHS "${DISCORD_SDK_LIB_DIR}" "${DISCORD_SDK_ROOT}/lib"
        PATH_SUFFIXES lib release x86
    )
    if (DISCORD_SDK_FALLBACK_LIB)
        set(DISCORD_LIB_PATH "${DISCORD_SDK_FALLBACK_LIB}")
    endif()
endif()

# Link the Social SDK library if available
if (EXISTS "${DISCORD_LIB_PATH}")
    target_link_libraries(DiscordSDKExample PRIVATE ${DISCORD_LIB_PATH})
    target_compile_definitions(DiscordSDKExample PRIVATE WOTLK_SDK_HAS_DISCORD_PARTNER_SDK=1)

    if (EXISTS "${DISCORD_SHARED_LIB}")
        add_custom_command(TARGET DiscordSDKExample POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DISCORD_SHARED_LIB}"
            "$<TARGET_FILE_DIR:DiscordSDKExample>"
            COMMENT "Copying Discord Social SDK shared library to target directory"
        )
    endif()
endif()
