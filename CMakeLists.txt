cmake_minimum_required(VERSION 3.15)

project(ScreenSaver VERSION 1.0.1)

include(CMake/CPM.cmake)

CPMAddPackage(
        NAME JUCE
        GIT_REPOSITORY git@github.com:juce-framework/JUCE.git
        GIT_TAG origin/master)

juce_add_gui_app(ScreenSaver
    PRODUCT_NAME "ScreenSaver"
    COMPANY_NAME "Rich Elmes"
    BUNDLE_ID com.richelmes.screensaver
    VERSION 1.0.0
)

target_sources(ScreenSaver PRIVATE
    Source/Main.cpp
    Source/SaverComponent.h
    Source/ImageComponent.h
    Source/ConfigComponent.h
    Source/Config.h
)

target_compile_definitions(ScreenSaver PRIVATE
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ScreenSaver,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ScreenSaver,JUCE_VERSION>"
)

target_link_libraries(ScreenSaver PRIVATE
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_graphics
    juce::juce_core
)

# Remove console window
set_target_properties(ScreenSaver PROPERTIES
    WIN32_EXECUTABLE TRUE
)

# Output .scr instead of .exe
add_custom_command(TARGET ScreenSaver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "$<TARGET_FILE:ScreenSaver>"
        "$<TARGET_FILE_DIR:ScreenSaver>/ScreenSaver.scr"
)