#include <windows.h> // <-- Add this first!
#define WIN32_LEAN_AND_MEAN

#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

#include "ConfigComponent.h"
#include "SaverComponent.h"

class ScreensaverApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override { return JUCE_APPLICATION_NAME_STRING; }

    const juce::String getApplicationVersion() override { return JUCE_APPLICATION_VERSION_STRING; }

    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        if (commandLine.isEmpty() || commandLine.startsWith("/s"))
        {
            runFullScreen();
        }
        else if (commandLine.startsWith("/c"))
        {
            showConfigDialog();
        }
        else if (commandLine.startsWith("/p"))
        {
            quit();
        }
        else
        {
            runFullScreen();
        }
    }

    void shutdown() override
    {
        savers.clear();
        config.reset();;
    }

    void systemRequestedQuit() override { quit(); }

private:
    std::vector<std::unique_ptr<SaverComponent>> savers;
    std::unique_ptr<ConfigWindow> config;

    void runFullScreen()
    {
        auto screens = juce::Desktop::getInstance().getDisplays();
        for (const auto& display : screens.displays)
        {
            auto saver = std::make_unique<SaverComponent>(false, [] { quit(); });
            auto screenBounds = display.totalArea;
            saver->setOpaque(true);
            saver->setBounds(screenBounds);
            saver->addToDesktop(0);
            saver->setVisible(true);
            saver->toFront(true);
            savers.push_back(std::move(saver));
        }
    }

    void showConfigDialog() { config = std::make_unique<ConfigWindow>(getApplicationName()); }
};

START_JUCE_APPLICATION(ScreensaverApplication)
