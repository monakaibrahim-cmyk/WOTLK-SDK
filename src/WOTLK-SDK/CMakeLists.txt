project(WotlkSDK)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

# WotlkSDK is strictly built as a static library (.lib).
# Dynamic DLLs are reserved for consumer mods and custom modules.
add_library(
    ${PROJECT_NAME} STATIC
        ${HEADERS}
        "Entry.cpp"
        "Extensions.h" "Extensions.cpp"
        "AutoLogin.h" "AutoLogin.cpp"
        "Discord.cpp"
        "WotlkSDK_C.cpp"
)

add_library(WotlkSDK::WotlkSDK ALIAS ${PROJECT_NAME})

target_include_directories(
    ${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
        WotlkSDK_Common
    PRIVATE
        shell32
)

# Discord Social SDK Linkage & DLL Deployment (via vcpkg)
if (WOTLK_SDK_HAVE_DISCORD_GAME_SDK)
    target_compile_definitions(${PROJECT_NAME} PUBLIC WOTLK_SDK_HAS_DISCORD_GAME_SDK=1)
    target_include_directories(${PROJECT_NAME} PUBLIC ${DISCORD_GAME_SDK_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${DISCORD_GAME_SDK_LIB})
    if (DISCORD_GAME_SDK_CPP_LIB)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${DISCORD_GAME_SDK_CPP_LIB})
    endif()

    find_file(DISCORD_GAME_SDK_DLL
        NAMES discord_game_sdk.dll
        PATH_SUFFIXES bin
    )
    if (DISCORD_GAME_SDK_DLL)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DISCORD_GAME_SDK_DLL}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
            COMMENT "Copying discord_game_sdk.dll to target directory"
        )
    endif()
endif()

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
        APP_NAME="${PROJECT_NAME}"
)

# Installation & Export
include(GNUInstallDirs)

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WOTLK_SDK_ARCH "x64")
else()
    set(WOTLK_SDK_ARCH "x86")
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT WotlkSDKTargets
    ARCHIVE DESTINATION "lib/${WOTLK_SDK_ARCH}"
    LIBRARY DESTINATION "lib/${WOTLK_SDK_ARCH}"
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
