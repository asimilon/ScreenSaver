#include <windows.h>  // <-- Add this first!
#define WIN32_LEAN_AND_MEAN

#include <juce_gui_basics/juce_gui_basics.h>
#include "SaverComponent.h"
#include "ConfigComponent.h"

class Test : public juce::Component {
public:
    ~Test() {
      logger.logMessage("exit");
    }

#if JUCE_DEBUG
    juce::FileLogger logger { juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDesktopDirectory).getChildFile("screensaver.log"),
                              "ScreenSaver log" };
#endif
};

class ScreensaverApplication : public juce::JUCEApplication
{
public:
    const juce::String getApplicationName() override       { return JUCE_APPLICATION_NAME_STRING; }
    const juce::String getApplicationVersion() override    { return JUCE_APPLICATION_VERSION_STRING; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise (const juce::String& commandLine) override
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
        saver = nullptr;
        config = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }
private:
    std::unique_ptr<SaverComponent> saver;
    std::unique_ptr<ConfigWindow> config;

    void runFullScreen()
    {
        auto mainScreen = juce::Desktop::getInstance().getDisplays().getMainDisplay();
        juce::Rectangle<int> screenBounds = mainScreen.totalArea;

        saver = std::make_unique<SaverComponent>(false, [this] { quit(); });
        saver->setOpaque(true);
        saver->setBounds(screenBounds);
        saver->addToDesktop(0);
        saver->setVisible(true);
        saver->toFront(true);
    }

    void showConfigDialog()
    {
        config.reset(new ConfigWindow(getApplicationName()));
    }
};

START_JUCE_APPLICATION(ScreensaverApplication)
