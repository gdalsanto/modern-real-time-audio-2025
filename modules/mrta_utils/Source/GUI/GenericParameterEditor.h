#pragma once

namespace mrta
{
// list of all parameters created in the audio processor side
// and will turn them into GUI components (knobs with labels)
class GenericParameterEditor : public juce::Component
{
public:
    GenericParameterEditor(mrta::ParameterManager& parameterManager,
                           int parameterWidgetHeight = 80,  // default height for the parameter widgets in pixels
                           const juce::StringArray& parameterIDs = {}); // list of parameters IDs of those that you want ot have in the component
    GenericParameterEditor() = delete;

    void paint(juce::Graphics&) override;
    void resized() override;

    const int parameterWidgetHeight;

private:
    juce::StringArray parameterIDs;
    std::vector<std::unique_ptr<juce::Component>> parameterComponents;
    std::vector<std::unique_ptr<juce::Label>> parameterLabels;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericParameterEditor)
};

}
