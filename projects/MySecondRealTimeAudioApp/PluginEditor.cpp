#include "PluginProcessor.h"
#include "PluginEditor.h"

MainProcessorEditor::MainProcessorEditor(MainProcessor& p) :
    AudioProcessorEditor(&p), audioProcessor(p),
    genericParameterEditor(audioProcessor.getParameterManager())
{
    int height = static_cast<int>(audioProcessor.getParameterManager().getParameters().size())
               * genericParameterEditor.parameterWidgetHeight;
    setSize(300, height);
    addAndMakeVisible(genericParameterEditor);  // to make your components as part of the editor. otherwise they will not be visible
}

MainProcessorEditor::~MainProcessorEditor()
{
}

void MainProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainProcessorEditor::resized()
{

    DBG("Width: " + juce::String(getWidth()) + " Height: " + juce::String(getHeight()));
    genericParameterEditor.setBounds(getLocalBounds());
}
