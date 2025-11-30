#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "SaverComponent.h"
#include "Config.h"

class ConfigLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawButtonBackground(juce::Graphics& g,
                              juce::Button& button,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        const auto w = static_cast<float>(button.getWidth());
        const auto h = static_cast<float>(button.getHeight());
        draw3Dbox(g, 0, 0, w, h, shouldDrawButtonAsDown, buttonBg);
    }

    void drawLinearSlider(juce::Graphics& g,
                          int x,
                          int y,
                          int width,
                          int height,
                          float sliderPos,
                          float minSliderPos,
                          float maxSliderPos,
                          const juce::Slider::SliderStyle style,
                          juce::Slider& slider) override
    {
        const auto bgX = static_cast<float>(x);
        const auto bgH = static_cast<float>(height) / 2;
        const auto bgY = static_cast<float>(y) + bgH / 2;
        const auto bgW = static_cast<float>(width);
        draw3Dbox(g, bgX, bgY, bgW, bgH, true, juce::Colours::grey);

        auto thumbWidth = getSliderThumbRadius(slider);
        sliderPos -= static_cast<float>(thumbWidth) / 2;
        draw3Dbox(g,
                  sliderPos,
                  static_cast<float>(y),
                  static_cast<float>(thumbWidth),
                  static_cast<float>(height),
                  false,
                  buttonBg);
    }

    void draw3Dbox(juce::Graphics& g, float x, float y, float w, float h, bool isDown, const juce::Colour& fillColour)
    {
        g.setColour(fillColour);
        g.fillRect(x, y, w, h);
        const auto x2 = x + w;
        const auto y2 = y + h;
        g.setColour(isDown ? buttonShadow : buttonHighlight);
        g.drawLine(x, y, x2, y, 1.0f);
        g.drawLine(x + 1, y + 1, x2 - 1, y + 1, 1.0f);
        g.drawLine(x, y, x, y2, 1.0f);
        g.drawLine(x + 1, y + 1, x + 1, y2 - 1, 1.0f);
        g.setColour(isDown ? buttonHighlight : buttonShadow);
        g.drawLine(x2, y, x2, y2, 1.0f);
        g.drawLine(x2 - 1, y + 1, x2 - 1, y2 - 1, 1.0f);
        g.drawLine(x, y2, x2, y2, 1.0f);
        g.drawLine(x + 1, y2 - 1, x2 - 1, y2 - 1, 1.0f);
    }

private:
    juce::Colour buttonBg { 0xff999999 };
    juce::Colour buttonHighlight { 0xffffffff };
    juce::Colour buttonShadow { 0xff000000 };
};

class ConfigComponent : public juce::Component
{
public:
    ConfigComponent()
    {
        setLookAndFeel(&lookAndFeel);

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
        addAndMakeVisible(timeValueLabel);
        timeValueLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::black);
        timeValueLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(timeSlider);
        timeSlider.setRange(3, 15, 1);
        timeSlider.setValue(Config::getTimeSetting());
        timeSlider.onValueChange = [this]
        {
            Config::getTimeSettingsFile().replaceWithText(juce::String(timeSlider.getValue()));
            updateLabel();
        };
        updateLabel();

        addAndMakeVisible(previewButton);
        previewButton.onClick = [this] { previewFullscreen(); };

        addAndMakeVisible(configPreview);

        setSize(500, 450);
    }

    ~ConfigComponent() override { setLookAndFeel(nullptr); }

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
        timeValueLabel.setBounds(timeBounds.removeFromLeft(100));
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

    void paint(juce::Graphics& g) override
    {
        auto bounds = configPreview.getBoundsInParent().expanded(1).toFloat();
        lookAndFeel.draw3Dbox(g, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), true, juce::Colours::black);
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

    void updateLabel() { timeValueLabel.setText(juce::String(timeSlider.getValue()) + " seconds", juce::dontSendNotification); };

    juce::Label imagesLabel { "images", "Images to use:" };
    juce::Label pathLabel;
    juce::TextButton setPathButton { "Choose" };
    juce::TextButton previewButton { "Preview" };
    juce::Label timeLabel { "time", "Time between images:" };
    juce::Label timeValueLabel;
    juce::Slider timeSlider { juce::Slider::SliderStyle::LinearHorizontal, juce::Slider::TextEntryBoxPosition::NoTextBox };
    SaverComponent configPreview { true, [] { } };

    std::unique_ptr<juce::FileChooser> filechooser;
    std::vector<std::unique_ptr<SaverComponent>> savers;

    ConfigLookAndFeel lookAndFeel;
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