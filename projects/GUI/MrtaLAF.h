#pragma once

#include <JuceHeader.h>

namespace GUI
{

class MrtaLAF : public juce::LookAndFeel_V4
{
public:
    MrtaLAF();
    ~MrtaLAF();

    // override the method that draws the rotary slider
    void drawRotarySlider(Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          Slider&) override;
    // if you want to add more customizations, you can override other methods here
    // e.g. drawButtonBackground , drawButtonText for the On / Off buttons
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MrtaLAF)
};

}
