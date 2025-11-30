#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <utility>
#include "Config.h"

class ImageComponent
    : public juce::Component
    , private juce::Timer
{
public:
    enum class State
    {
        None,
        Loading,
        Init,
        Waiting,
        FadeIn,
        Moving,
        FadeOut,
        Finished
    };

    explicit ImageComponent(const juce::File& path, bool firstInQueue, std::function<void()> fadeoutCallback)
        : isFirstInQueue(firstInQueue)
        , onFadeOut(std::move(fadeoutCallback))
    {
        setInterceptsMouseClicks(false, false);
        juce::Thread::launch(juce::Thread::Priority::normal,
                             [this, path]
                             {
                                 state = State::Loading;
                                 timeToShow = Config::getTimeSetting() * 60;
                                 image = juce::ImageCache::getFromFile(path);
                                 state = State::Init;
                             });
        startTimerHz(60);
    }

    void paint(juce::Graphics& g) override
    {
        if (state == State::FadeIn || state == State::Moving || state == State::FadeOut)
        {
            g.setOpacity(alpha);
            g.drawImage(image, targetArea);
        }
    }

    void startFadeIn() { state = State::FadeIn; }

    bool isFinished() const { return state == State::Finished; }

private:
    static float randomInRange(juce::Random& r, float min, float max)
    {
        return r.nextFloat() * (max - min) + min;
    }

    void timerCallback() override
    {
        switch (state)
        {
            case State::Init:
                zoom = 1.3f + randomInRange(random, -0.1f, 0.1f);
                zoomSpeed = randomInRange(random, -0.0003f, 0.0003f);
                requiredZoomSpeed = zoomSpeed;
                flippingZoomDirection = false;
                panX = randomInRange(random, 0.1f, 0.9f);
                panY = randomInRange(random, 0.1f, 0.9f);
                panSpeedX = randomInRange(random, -0.0005f, 0.0005f);
                panSpeedY = randomInRange(random, -0.0005f, 0.0005f);
                requiredPanSpeedX = panSpeedX;
                requiredPanSpeedY = panSpeedY;
                flippingDirectionX = false;
                flippingDirectionY = false;
                state = isFirstInQueue ? State::FadeIn : State::Waiting;
                break;
            case State::FadeIn:
                alpha += 0.01f;
                if (alpha >= 1.0f)
                {
                    alpha = 1.0f;
                    state = State::Moving;
                }
                updateAnimation();
                break;
            case State::Moving:
                updateAnimation();
                if (--timeToShow == 0)
                {
                    state = State::FadeOut;
                    onFadeOut();
                }
                break;
            case State::FadeOut:
                alpha -= 0.01f;
                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    state = State::Finished;
                }
                updateAnimation();
                break;
            default:
                break;
        }
        repaint();
    }

    void updateAnimation()
    {
        zoom += zoomSpeed;

        if ((zoom > 1.6f || zoom < 1.1f) && !flippingZoomDirection) {
            requiredZoomSpeed = zoomSpeed;
            flippingZoomDirection = true;
        }
        if (flippingZoomDirection) {
            const auto speedDelta = requiredZoomSpeed - zoomSpeed;
            zoomSpeed += speedDelta * 0.01f;
            if (juce::approximatelyEqual(zoomSpeed, requiredZoomSpeed)) {
                zoomSpeed = requiredZoomSpeed;
                flippingZoomDirection = false;
            }
        }

        panX += panSpeedX;
        panY += panSpeedY;

        if ((panX < 0.0f || panX > 1.0f) && !flippingDirectionX) {
            requiredPanSpeedX = -panSpeedX;
            flippingDirectionX = true;
        }
        if ((panY < 0.0f || panY > 1.0f) && !flippingDirectionY) {
            requiredPanSpeedY = -panSpeedY;
            flippingDirectionY = true;
        }

        if (flippingDirectionX) {
            const auto speedDelta = requiredPanSpeedX - panSpeedX;
            panSpeedX += speedDelta * 0.01f;
            if (juce::approximatelyEqual(panSpeedX, requiredPanSpeedX)) {
                panSpeedX = requiredPanSpeedX;
                flippingDirectionX = false;
            }
        }
        if (flippingDirectionY) {
            const auto speedDelta = requiredPanSpeedY - panSpeedY;
            panSpeedY += speedDelta * 0.01f;
            if (juce::approximatelyEqual(panSpeedY, requiredPanSpeedY)) {
                panSpeedY = requiredPanSpeedY;
                flippingDirectionY = false;
            }
        }

        // Get image and component aspect ratios
        auto imageAspect = (float) image.getWidth() / (float) image.getHeight();
        auto compBounds = getLocalBounds().toFloat();
        auto compAspect = compBounds.getWidth() / compBounds.getHeight();

        // Determine the base rectangle (fit image into component)
        juce::Rectangle<float> baseArea;
        if (imageAspect > compAspect)
            baseArea = { 0.0f, 0.0f, compBounds.getWidth(), compBounds.getWidth() / imageAspect };
        else
            baseArea = { 0.0f, 0.0f, compBounds.getHeight() * imageAspect, compBounds.getHeight() };

        baseArea = baseArea.withCentre(compBounds.getCentre());

        // Apply zoom (centered around pan position)
        auto zoomed = baseArea.withSizeKeepingCentre(baseArea.getWidth() * zoom, baseArea.getHeight() * zoom);

        // Pan within the zoomed area
        float maxPanX = (zoomed.getWidth() - baseArea.getWidth()) / 2.0f;
        float maxPanY = (zoomed.getHeight() - baseArea.getHeight()) / 2.0f;

        targetArea = zoomed.translated((panX - 0.5f) * 2 * maxPanX, (panY - 0.5f) * 2 * maxPanY);
    }

    bool isFirstInQueue;
    std::atomic<State> state { State::None };
    float alpha { 0.0f };
    size_t timeToShow;

    juce::Rectangle<float> targetArea;

    float zoom{};
    float zoomSpeed{};
    float requiredZoomSpeed{};
    bool flippingZoomDirection = false;
    float panX{}, panY{};
    float panSpeedX{}, panSpeedY{};
    float requiredPanSpeedX{}, requiredPanSpeedY{};
    bool flippingDirectionX = false, flippingDirectionY = false;

    juce::Image image;
    juce::Random random;

    std::function<void()> onFadeOut;
};