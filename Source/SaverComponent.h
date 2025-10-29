#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SaverComponent : public juce::Component,
                       private juce::Timer
{
public:
    explicit SaverComponent(bool isPreview, std::function<void()> onExitFn)
        : onExit(std::move(onExitFn))
        , preview(isPreview)
    {
        setWantsKeyboardFocus(true);
        setMouseClickGrabsKeyboardFocus(true);
        if (!preview) setMouseCursor(juce::MouseCursor::NoCursor);
        startTimerHz(60);
        lastMousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);
        g.setColour(juce::Colours::white);
        g.setFont(40.0f);
        g.drawFittedText("JUCE Screensaver", getLocalBounds(), juce::Justification::centred, 1);
    }

private:
    juce::Point<float> lastMousePos;
    std::function<void()> onExit;
    bool preview;

    void timerCallback() override
    {
        auto mousePos = juce::Desktop::getInstance().getMainMouseSource().getScreenPosition();
        if (!preview && mousePos.getDistanceFrom(lastMousePos) > 5.0f)
            triggerExit();

        repaint();
    }

    void mouseDown(const juce::MouseEvent&) override   { if (!preview) triggerExit(); }
    bool keyPressed(const juce::KeyPress&) override    { if (!preview) triggerExit(); return !preview; }

    void triggerExit()
    {
        if (onExit)
            onExit();
    }
};