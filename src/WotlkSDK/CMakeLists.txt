project(WotlkSDK)

file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")

# Default to STATIC library (.lib) for SDK consumers, with option for standalone SHARED (.dll)
option(BUILD_SHARED_LIBS "Build WotlkSDK as a shared library (.dll) instead of static library (.lib)" OFF)

add_library(
    ${PROJECT_NAME}
        ${HEADERS}
        "Entry.cpp"
        "Extensions.h" "Extensions.cpp"
        "AutoLogin.h" "AutoLogin.cpp"
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
        "${CMAKE_SOURCE_DIR}/include"
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
)
