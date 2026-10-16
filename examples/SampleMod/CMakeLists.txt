project(SampleMod LANGUAGES CXX)

add_library(SampleMod SHARED
    Main.cpp
)

target_link_libraries(SampleMod PRIVATE
    WotlkSDK::WotlkSDK
)

target_compile_definitions(SampleMod PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)
