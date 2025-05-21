#include "PluginEditor.h"
#include "mrta_utils/Source/GUI/GenericParameterEditor.h"

MySynthesizerProcessorEditor::MySynthesizerProcessorEditor(MySynthesizerProcessor& p) :
    juce::AudioProcessorEditor(p), audioProcessor(p),
    paramEditor(audioProcessor.getParamManager())
{
    addAndMakeVisible(paramEditor);
    setSize(300, 1000);
}


MySynthesizerProcessorEditor::~MySynthesizerProcessorEditor()
{
}

//==============================================================================
void MySynthesizerProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MySynthesizerProcessorEditor::resized()
{
    paramEditor.setBounds(getLocalBounds());
}