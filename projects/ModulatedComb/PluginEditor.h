#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class CombAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    CombAudioProcessorEditor (CombAudioProcessor&);
    ~CombAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CombAudioProcessor& audioProcessor;
    mrta::GenericParameterEditor genericParameterEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombAudioProcessorEditor)
};