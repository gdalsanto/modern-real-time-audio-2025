#include "PluginProcessor.h"
#include "PluginEditor.h"

static const std::vector<mrta::ParameterInfo> ParameterInfos
{
    // bool constructor will be called
    { Param::ID::Enabled,   Param::Name::Enabled, "Off", "On", true },    
    // float constructor will be called 
    {   
        Param::ID::Frequency, 
        Param::Name::Frequency, 
        "Hz", 
        1000.f,     // default value
        20.f,       // min value
        20000.f,    // max value
        1.f,        // step value
        0.3f        // skew value
    },
    { Param::ID::Depth, Param::Name::Depth, "Lin", 0.0f, 0.0f, 1.f, 0.01f, 0.3f  }
};


/* constructor: Register all callback lambda function for the parameters
The callbacks will be run at every parameter change event.
DBG will print a string - this is usefull for debugging*/
MainProcessor::MainProcessor() :
    parameterManager(*this, ProjectInfo::projectName, ParameterInfos)
{
    // If the effect is enabled, the modulation depth will be set to the current value
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this] (float value, bool /*forced*/)
    {
        DBG(Param::Name::Enabled + ": " + juce::String { value });     // debugging 
        float defaultFreq { 1000.f };
        if (value > 0.f) {
            float modulationValue = depth.getCurrentValue() * 
                                     std::cos(2.0 * juce::MathConstants<float>::pi * defaultFreq / this->samplingFreq * this->n);
            modulation.setCurrentAndTargetValue(modulationValue);
        } else {
            modulation.setCurrentAndTargetValue(0.f);
        }
    });

    parameterManager.registerParameterCallback(Param::ID::Frequency,
    [this] (float value, bool /*forced*/)   // 
    {
        DBG(Param::Name::Frequency + ": " + juce::String { value });
        float modulationValue = depth.getCurrentValue() * 
                                 std::cos(2.0 * juce::MathConstants<float>::pi * value / this->samplingFreq * this->n);
        modulation.setTargetValue(modulationValue);
    });

    parameterManager.registerParameterCallback(Param::ID::Depth,
    [this] (float value, bool )
    {
        DBG(Param::Name::Depth + ": " + juce::String { value });
        depth.setTargetValue(value); // smooth the gain change
    });
}

MainProcessor::~MainProcessor()
{
}
// prepareToPlay and processBlock are the key two methods of the project 
void MainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::uint32 numChannels { static_cast<juce::uint32>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    this->samplingFreq = static_cast<int>(sampleRate);
    this->n = 0;
    depth.reset(sampleRate, 0.01f); // sets the sample rate and the duration of the ramp 
    modulation.reset(sampleRate, 0.01f); // sets the sample rate and the duration of the ramp
    parameterManager.updateParameters(true);
}

void MainProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;    // set the denormals to zero
    parameterManager.updateParameters();

    {
        juce::dsp::AudioBlock<float> audioBlock(buffer.getArrayOfWritePointers(), buffer.getNumChannels(), buffer.getNumSamples());
        juce::dsp::ProcessContextReplacing<float> ctx(audioBlock);
    }
    this->n += buffer.getNumSamples();
    modulation.applyGain(buffer, buffer.getNumSamples());
}

void MainProcessor::releaseResources()
{
    this->n = 0;
    // filter.reset();
}

void MainProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void MainProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

juce::AudioProcessorEditor* MainProcessor::createEditor()
{
    return new MainProcessorEditor(*this);
}

//==============================================================================
const juce::String MainProcessor::getName() const { return JucePlugin_Name; }
bool MainProcessor::acceptsMidi() const { return false; }
bool MainProcessor::producesMidi() const { return false; }
bool MainProcessor::isMidiEffect() const { return false; }
double MainProcessor::getTailLengthSeconds() const { return 0.0; }
int MainProcessor::getNumPrograms() { return 1; }
int MainProcessor::getCurrentProgram() { return 0; }
void MainProcessor::setCurrentProgram (int) { }
const juce::String MainProcessor::getProgramName(int) { return {}; }
void MainProcessor::changeProgramName(int, const juce::String&) { }
bool MainProcessor::hasEditor() const { return true; }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MainProcessor();
}
