project(SampleModC LANGUAGES C)

add_library(SampleModC SHARED
    Main.c
)

target_link_libraries(SampleModC PRIVATE
    WotlkSDK::WotlkSDK
)

target_compile_definitions(SampleModC PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
