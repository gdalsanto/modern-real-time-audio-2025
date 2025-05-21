#pragma once

#include "PluginProcessor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

class MySynthesizerProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MySynthesizerProcessorEditor(MySynthesizerProcessor&);
    ~MySynthesizerProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MySynthesizerProcessor& audioProcessor;
    mrta::GenericParameterEditor paramEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySynthesizerProcessorEditor)
};