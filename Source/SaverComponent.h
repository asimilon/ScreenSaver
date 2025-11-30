#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <numeric>
#include <random>

#include "Config.h"
#include "ImageComponent.h"

class SaverComponent
    : public juce::Component
    , private juce::Timer
{
public:
    explicit SaverComponent(bool isPreview, std::function<void()> onExitFn)
        : onExit(std::move(onExitFn))
        , preview(isPreview)
    {
        reset();

        setWantsKeyboardFocus(true);
        setMouseClickGrabsKeyboardFocus(true);
        if (!preview)
            setMouseCursor(juce::MouseCursor::NoCursor);
        lastMousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
        startTimerHz(60);
    }

    void reset()
    {
        imagePaths.clear();
        imageIndices.clear();
        auto settingsFile = Config::getPathSettingsFile();
        auto imagesPath = juce::File(settingsFile.loadFileAsString());
        if (imagesPath.exists())
        {
            imagePaths = imagesPath.findChildFiles(juce::File::TypesOfFileToFind::findFiles, true, "*.jpg");
        }
    }

    void paint(juce::Graphics& g) override { g.fillAll(juce::Colours::black); }

private:
    juce::Point<float> lastMousePos;
    std::function<void()> onExit;
    bool preview;
    juce::Array<juce::File> imagePaths;
    std::vector<int> imageIndices;
    std::vector<std::unique_ptr<ImageComponent>> imageComponents;

    juce::Random random;

    void timerCallback() override
    {
        auto mousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
        if (!preview && mousePos.getDistanceFrom(lastMousePos) > 5.0f)
            triggerExit();

        if (imageIndices.empty())
        {
            imageIndices.resize(imagePaths.size());
            std::iota(imageIndices.begin(), imageIndices.end(), 0);
            std::random_device rd;
            std::mt19937 rng(rd());
            std::shuffle(imageIndices.begin(), imageIndices.end(), rng);
        }

        if (imageComponents.empty() && !imageIndices.empty())
        {
            const auto& imagePath = imagePaths[imageIndices.front()];
            imageIndices.pop_back();
            auto imageComp = std::make_unique<ImageComponent>(imagePath, true, [this] { triggerNext(); });
            imageComp->setBounds(getLocalBounds());
            addAndMakeVisible(*imageComp);
            imageComponents.push_back(std::move(imageComp));
        }
        else if (imageComponents.size() < 2 && !imageIndices.empty())
        {
            const auto& imagePath = imagePaths[imageIndices.back()];
            imageIndices.pop_back();
            auto imageComp = std::make_unique<ImageComponent>(imagePath, false, [this] { triggerNext(); });
            imageComp->setBounds(getLocalBounds());
            addAndMakeVisible(*imageComp);
            imageComponents.push_back(std::move(imageComp));
        }
        if (!imageComponents.empty())
        {
            if (imageComponents.front()->isFinished())
            {
                imageComponents.erase(imageComponents.begin());
            }
        }

        repaint();
    }

    void triggerNext() { imageComponents.back()->startFadeIn(); }

    void mouseDown(const juce::MouseEvent&) override
    {
        if (!preview)
            triggerExit();
    }

    bool keyPressed(const juce::KeyPress&) override
    {
        if (!preview)
            triggerExit();
        return !preview;
    }

    void triggerExit()
    {
        if (onExit)
            onExit();
    }
};