#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "SaverComponent.h"
#include "Config.h"

class ConfigComponent : public juce::Component
{
public:
    ConfigComponent()
    {
        auto pathSettingFile = Config::getPathSettingsFile();
        if (pathSettingFile.existsAsFile())
            pathLabel.setText(pathSettingFile.loadFileAsString(), juce::dontSendNotification);
        else
            pathLabel.setText("<choose images folder>", juce::dontSendNotification);

        addAndMakeVisible(imagesLabel);
        imagesLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

        addAndMakeVisible(pathLabel);
        pathLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);

        addAndMakeVisible(setPathButton);
        setPathButton.onClick = [this]
        {
            filechooser = std::make_unique<juce::FileChooser>(
                "Choose images folder",
                juce::File::getSpecialLocation(juce::File::SpecialLocationType::userPicturesDirectory));
            filechooser->launchAsync(juce::FileBrowserComponent::FileChooserFlags::canSelectDirectories,
                                     [this](const juce::FileChooser& fc)
                                     {
                                         if (fc.getResult().exists())
                                         {
                                             auto pathSettingFile = Config::getPathSettingsFile();
                                             (void) pathSettingFile.replaceWithText(fc.getResult().getFullPathName());
                                             pathLabel.setText(fc.getResult().getFullPathName(), juce::dontSendNotification);
                                             configPreview.reset();
                                         }
                                     });
        };

        addAndMakeVisible(timeLabel);
        timeLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        addAndMakeVisible(timeSlider);
        timeSlider.setRange(3, 15, 1);
        timeSlider.setValue(Config::getTimeSetting());
        timeSlider.onValueChange = [this]
        {
            Config::getTimeSettingsFile().replaceWithText(juce::String(timeSlider.getValue()));
        };

        addAndMakeVisible(previewButton);
        previewButton.onClick = [this] { previewFullscreen(); };

        addAndMakeVisible(configPreview);

        setSize(500, 450);
    }

    void resized() override
    {
        auto bounds = getBounds().reduced(20);

        auto imagesBounds = bounds.removeFromTop(30);
        imagesLabel.setBounds(imagesBounds);

        bounds.removeFromTop(5);

        auto pathBounds = bounds.removeFromTop(30);

        setPathButton.setBounds(pathBounds.removeFromLeft(100));
        pathBounds.removeFromLeft(20);
        pathLabel.setBounds(pathBounds);

        bounds.removeFromTop(5);

        auto timeBounds = bounds.removeFromTop(30);
        timeLabel.setBounds(timeBounds.removeFromLeft(150));
        timeBounds.removeFromLeft(10);
        timeSlider.setBounds(timeBounds);

        bounds.removeFromTop(5);

        previewButton.setBounds(bounds.removeFromTop(30).removeFromLeft(100));

        auto previewBounds = bounds.reduced(20);

        constexpr auto ratio = 16.f / 9.f;

        configPreview.setBounds(
            previewBounds.withSizeKeepingCentre(static_cast<int>(static_cast<float>(previewBounds.getHeight()) * ratio),
                                                previewBounds.getHeight()));
    }

private:
    void previewFullscreen()
    {
        const auto& displays = juce::Desktop::getInstance().getDisplays().displays;
        for (const auto& display : displays)
        {
            auto saver = std::make_unique<SaverComponent>(false, [this] { stopPreview(); });
            auto screenBounds = display.totalArea;
            saver->setOpaque(true);
            saver->setBounds(screenBounds);
            saver->addToDesktop(0);
            saver->setVisible(true);
            saver->toFront(true);
            savers.push_back(std::move(saver));
        }
    }

    void stopPreview() { savers.clear(); }

    juce::Label imagesLabel { "images", "Images to use:" };
    juce::Label pathLabel;
    juce::TextButton setPathButton { "Choose" };
    juce::TextButton previewButton { "Preview" };
    juce::Label timeLabel { "time", "Time between images:" };
    juce::Slider timeSlider { juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::TextBoxLeft };
    SaverComponent configPreview { true, [] { } };

    std::unique_ptr<juce::FileChooser> filechooser;
    std::vector<std::unique_ptr<SaverComponent>> savers;
};

class ConfigWindow : public juce::DocumentWindow
{
public:
    explicit ConfigWindow(const juce::String& name)
        : juce::DocumentWindow(name, juce::Colours::grey, juce::DocumentWindow::allButtons)
    {
        setUsingNativeTitleBar(true);
        setContentOwned(new ConfigComponent(), true);
        DocumentWindow::setVisible(true);
        centreWithSize(getWidth(), getHeight());
    }

    void closeButtonPressed() override
    {
        // This is called when the user tries to close this window. Here, we'll just
        // ask the app to quit when this happens, but you can change this to do
        // whatever you need.
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConfigWindow)
};