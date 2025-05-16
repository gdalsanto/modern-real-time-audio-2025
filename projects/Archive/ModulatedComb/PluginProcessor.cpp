#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>

static const std::vector<mrta::ParameterInfo> Parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Ranges::EnabledOff, Param::Ranges::EnabledOn, true },
    { Param::ID::Offset,   Param::Name::Offset,   Param::Units::Ms,  2.f,  Param::Ranges::OffsetMin,   Param::Ranges::OffsetMax,   Param::Ranges::OffsetInc,   Param::Ranges::OffsetSkw },
    { Param::ID::Depth,    Param::Name::Depth,    Param::Units::Ms,  2.f,  Param::Ranges::DepthMin,    Param::Ranges::DepthMax,    Param::Ranges::DepthInc,    Param::Ranges::DepthSkw },
    { Param::ID::Rate,     Param::Name::Rate,     Param::Units::Hz,  0.5f, Param::Ranges::RateMin,     Param::Ranges::RateMax,     Param::Ranges::RateInc,     Param::Ranges::RateSkw },
    { Param::ID::ModType,  Param::Name::ModType,  Param::Ranges::ModLabels, 0 },
    { Param::ID::Feedback, Param::Name::Feedback, Param::Units::Lin,  0.5f, Param::Ranges::FBMin,    Param::Ranges::FBMax,       Param::Ranges::FBInc,       Param::Ranges::FBSkw },
    { Param::ID::Wet,      Param::Name::Wet,      Param::Units::Lin,  0.f, Param::Ranges::WetMin,   Param::Ranges::WetMax,      Param::Ranges::WetInc,      Param::Ranges::WetSkw }
};  

CombAudioProcessor::CombAudioProcessor() :
    parameterManager(*this, ProjectInfo::projectName, Parameters),
    comb(MaxDelaySizeMs, 2),
    enableRamp(0.05f),
    wetRamp(0.0f),
    dryRamp(1.0f)
{
    parameterManager.registerParameterCallback(Param::ID::Enabled,
    [this](float newValue, bool force)
    {
        enabled = newValue > 0.5f;
        enableRamp.setTarget(enabled ? 1.f : 0.f, force);
    });

    parameterManager.registerParameterCallback(Param::ID::Offset,
    [this] (float newValue, bool /*force*/)
    {
        comb.setOffset(newValue);
    });

    parameterManager.registerParameterCallback(Param::ID::Depth,
    [this](float newValue, bool /*force*/)
    {
        comb.setDepth(newValue);
    });

    parameterManager.registerParameterCallback(Param::ID::Rate,
    [this] (float newValue, bool /*force*/)
    {
        comb.setModulationRate(newValue);
    });

    parameterManager.registerParameterCallback(Param::ID::ModType,
    [this](float newValue, bool /*force*/)
    {
        DSP::Comb::ModulationType modType = static_cast<DSP::Comb::ModulationType>(std::round(newValue));
        comb.setModulationType(std::min(std::max(modType, DSP::Comb::Sin), DSP::Comb::Tri));
    });

    parameterManager.registerParameterCallback(Param::ID::Feedback, 
    [this] (float newValue, bool /*force*/)
    {
        comb.setFeedbackGain(newValue);
    });

    parameterManager.registerParameterCallback(Param::ID::Wet,
    [this] (float newValue, bool /*force*/)
    {
        wetRamp.setTarget(newValue);
        dryRamp.setTarget(1.f - newValue);
    });
    
}

CombAudioProcessor::~CombAudioProcessor()
{
}

void CombAudioProcessor::prepareToPlay(double newSampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };

    comb.prepare(newSampleRate, MaxDelaySizeMs, numChannels);
    enableRamp.prepare(newSampleRate, true, enabled ? 1.f : 0.f);
    wetRamp.prepare(newSampleRate, true, 0.f);

    parameterManager.updateParameters(true);

    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void CombAudioProcessor::releaseResources()
{
    comb.clear();
}

void CombAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    parameterManager.updateParameters();

    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };

    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));

    comb.process(fxBuffer.getArrayOfWritePointers(), fxBuffer.getArrayOfReadPointers(), numChannels, numSamples);
    wetRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);
    
    enableRamp.applyGain(fxBuffer.getArrayOfWritePointers(), numChannels, numSamples);

    dryRamp.applyGain(buffer.getArrayOfWritePointers(), numChannels, numSamples);
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
}

void CombAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    parameterManager.getStateInformation(destData);
}

void CombAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    parameterManager.setStateInformation(data, sizeInBytes);
}

//==============================================================================
bool CombAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* CombAudioProcessor::createEditor() { return new CombAudioProcessorEditor(*this); }
const juce::String CombAudioProcessor::getName() const { return JucePlugin_Name; }
bool CombAudioProcessor::acceptsMidi() const { return false; }
bool CombAudioProcessor::producesMidi() const { return false; }
bool CombAudioProcessor::isMidiEffect() const { return false; }
double CombAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int CombAudioProcessor::getNumPrograms() { return 1; }
int CombAudioProcessor::getCurrentProgram() { return 0; }
void CombAudioProcessor::setCurrentProgram(int) { }
const juce::String CombAudioProcessor::getProgramName (int) { return {}; }
void CombAudioProcessor::changeProgramName (int, const juce::String&) { }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CombAudioProcessor();
}