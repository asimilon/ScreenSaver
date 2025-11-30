#pragma once

#include <juce_core/juce_core.h>

namespace Config
{
    static juce::File getPathSettingsFile()
    {
        auto settingsFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory)
                                  .getChildFile("ScreenSaver");
        if (!settingsFolder.exists())
            (void) settingsFolder.createDirectory();
        return settingsFolder.getChildFile("images.path");
    }

    static juce::File getTimeSettingsFile()
    {
        auto settingsFolder = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory)
                                  .getChildFile("ScreenSaver");
        if (!settingsFolder.exists())
            (void) settingsFolder.createDirectory();
        return settingsFolder.getChildFile("images.time");
    }

    static int getTimeSetting()
    {
        auto settingsFile = getTimeSettingsFile();
        if (settingsFile.existsAsFile())
            return settingsFile.loadFileAsString().getIntValue();
        return 5;
    }
} // namespace Config