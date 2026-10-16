project(WotlkSDK)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

add_library(
    ${PROJECT_NAME}
        ${HEADERS}
        "Entry.cpp"
        "Extensions.h" "Extensions.cpp"
        "AutoLogin.h" "AutoLogin.cpp"
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

target_compile_definitions(
    ${PROJECT_NAME} PRIVATE
        APP_NAME="${PROJECT_NAME}"
        $<$<BOOL:${BUILD_SHARED_LIBS}>:WOTLK_SDK_BUILD_SHARED>
        $<$<BOOL:${BUILD_SHARED_LIBS}>:WOTLK_SDK_EXPORTS>
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
    RUNTIME DESTINATION "bin/${WOTLK_SDK_ARCH}"
)

install(
    DIRECTORY "${CMAKE_SOURCE_DIR}/include/"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
