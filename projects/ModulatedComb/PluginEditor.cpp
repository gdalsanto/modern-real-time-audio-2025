#include "PluginProcessor.h"
#include "PluginEditor.h"

CombAudioProcessorEditor::CombAudioProcessorEditor(CombAudioProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    unsigned int numParams { static_cast<unsigned int>(audioProcessor.getParameterManager().getParameters().size()) };
    unsigned int paramHeight { static_cast<unsigned int>(genericParameterEditor.parameterWidgetHeight) };

    addAndMakeVisible(genericParameterEditor);
    setSize(300, numParams * paramHeight);
}

CombAudioProcessorEditor::~CombAudioProcessorEditor()
{
}

void CombAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void CombAudioProcessorEditor::resized()
{
    genericParameterEditor.setBounds(getLocalBounds());
}