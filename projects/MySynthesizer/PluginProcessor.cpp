#include "PluginProcessor.h"
#include "JuceHeader.h"
#include "Oscillator.h"
#include "PluginEditor.h"
#include "SynthVoice.h"
#include "mrta_utils/Source/Parameter/ParameterInfo.h"
#include "mrta_utils/Source/Parameter/ParameterManager.h"

void modeMix(float mode, float& lpf, float& bpf, float& hpf)
{
    mode = std::clamp(mode, -1.f, 1.f);
    lpf = std::fmax(-mode, 0.f);
    bpf = std::fmax(1.f - std::fabs(mode), 0.f);
    hpf = std::fmax(mode, 0.f);
}

static const std::vector<mrta::ParameterInfo> paramVector
{
    // Filter parameters
    { Param::ID::Freq, Param::Name::Freq, "Hz", 1000.f, Param::Ranges::FreqMin, Param::Ranges::FreqMax, Param::Ranges::FreqInc, Param::Ranges::FreqSkw },
    { Param::ID::Reso, Param::Name::Reso, "", 1.0f, Param::Ranges::ResoMin, Param::Ranges::ResoMax, Param::Ranges::ResoInc, Param::Ranges::ResoSkw },
    { Param::ID::Mode, Param::Name::Mode, "", 0.0f,  Param::Ranges::ModeMin, Param::Ranges::ModeMax, Param::Ranges::ModeInc, Param::Ranges::ModeSkw },

    // Envelope parameters
    { Param::ID::Trigger, Param::Name::Trigger, Param::Ranges::TriggerOff, Param::Ranges::TriggerOn, false },
    { Param::ID::AttTime, Param::Name::AttTime, "ms", 100.f, Param::Ranges::AttTimeMin, Param::Ranges::AttTimeMax, Param::Ranges::AttTimeInc, Param::Ranges::AttTimeSkw },
    { Param::ID::DecTime, Param::Name::DecTime, "ms", 100.f, Param::Ranges::DecTimeMin, Param::Ranges::DecTimeMax, Param::Ranges::DecTimeInc, Param::Ranges::DecTimeSkw },
    { Param::ID::SusLevel, Param::Name::SusLevel, "", 0.7f, Param::Ranges::SusMin, Param::Ranges::SusMax, Param::Ranges::SusInc, Param::Ranges::SusSkw },
    { Param::ID::RelTime, Param::Name::RelTime, "ms", 100.f, Param::Ranges::RelTimeMin, Param::Ranges::RelTimeMax, Param::Ranges::RelTimeInc, Param::Ranges::RelTimeSkw },
    // Source parameters
    { Param::ID::WaveType, Param::Name::WaveType, Param::Ranges::WaveType, 0 }
};

MySynthesizerProcessor::MySynthesizerProcessor() :
    paramManager(*this, ProjectInfo::projectName, paramVector)
{
    voice = new DSP::SynthVoice();
    synth.addSound(new DSP::SynthSound());
    synth.addVoice(voice);

    // Filter parameters
    paramManager.registerParameterCallback(Param::ID::Freq,
    [this] (float value, bool force)
    {
        freqHz = value;
        freqRamp.setTarget(value, force);
    });


    paramManager.registerParameterCallback(Param::ID::Reso,
    [this] (float value, bool force)
    {
        reso = value;
        resoRamp.setTarget(value, force);
    });

    paramManager.registerParameterCallback(Param::ID::Mode,
    [this] (float value, bool force)
    {
        mode = value;
        float lpf(0.f), bpf(0.f), hpf(0.f);
        modeMix(mode, lpf, bpf, hpf);
        lpfRamp.setTarget(lpf, force);
        bpfRamp.setTarget(bpf, force);
        hpfRamp.setTarget(hpf, force);
    });

    // Envlope generator 
    paramManager.registerParameterCallback(Param::ID::Trigger,
    [this] (float value, bool /*force*/)
    {
        if (value > 0.5f)
            env.start();
        else
            env.end();
    });

    paramManager.registerParameterCallback(Param::ID::AttTime,
    [this] (float value, bool /*force*/)
    {
        env.setAttackTime(value);
    });

    paramManager.registerParameterCallback(Param::ID::DecTime,
    [this] (float value, bool /*force*/)
    {
        env.setDecayTime(value);
    });

    paramManager.registerParameterCallback(Param::ID::SusLevel,
    [this] (float value, bool /*force*/)
    {
        env.setSustainLevel(value);
    });

    paramManager.registerParameterCallback(Param::ID::RelTime,
    [this] (float value, bool /*force*/)
    {
        env.setReleaseTime(value);
    });

    // Source parameters
    paramManager.registerParameterCallback(Param::ID::WaveType,
    [this] (float value, bool /*force*/)
    {
        DSP::Oscillator::OscType type = static_cast<DSP::Oscillator::OscType>(std::rint(value));
        voice->setWaveType(type);
    });
}

MySynthesizerProcessor::~MySynthesizerProcessor()
{
}

void MySynthesizerProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    paramManager.updateParameters(true);
    env.prepare(sampleRate);

    // SVF
    SVF.prepare(sampleRate);
    freqRamp.prepare(sampleRate, true, freqHz);
    resoRamp.prepare(sampleRate, true, reso);

    float lpf, bpf, hpf;
    modeMix(mode, lpf, bpf, hpf);
    lpfRamp.prepare(sampleRate, true, lpf);
    bpfRamp.prepare(sampleRate, true, bpf);
    hpfRamp.prepare(sampleRate, true, hpf);

    // resize the aux buffers
    freqInBuffer.setSize(1, samplesPerBlock);
    resoInBuffer.setSize(1, samplesPerBlock);
    lpfOutBuffer.setSize(2, samplesPerBlock);
    bpfOutBuffer.setSize(2, samplesPerBlock);
    hpfOutBuffer.setSize(2, samplesPerBlock);
}

void MySynthesizerProcessor::releaseResources()
{
}

void MySynthesizerProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    paramManager.updateParameters();

    // clear all aux buffers
    freqInBuffer.clear();
    resoInBuffer.clear();
    lpfOutBuffer.clear();
    bpfOutBuffer.clear();
    hpfOutBuffer.clear();

    // get the freq and reso controls
    freqRamp.applySum(freqInBuffer.getWritePointer(0), buffer.getNumSamples());
    resoRamp.applySum(resoInBuffer.getWritePointer(0), buffer.getNumSamples());


    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    env.process(buffer.getWritePointer(0), buffer.getNumSamples());

    SVF.process(lpfOutBuffer.getWritePointer(0),
                    bpfOutBuffer.getWritePointer(0),
                    hpfOutBuffer.getWritePointer(0),
                    buffer.getReadPointer(0),
                    freqInBuffer.getReadPointer(0),
                    resoInBuffer.getReadPointer(0),
                    buffer.getNumSamples());

}

void MySynthesizerProcessor::getStateInformation(juce::MemoryBlock& destData)
{
}

void MySynthesizerProcessor::setStateInformation(const void* data, int sizeInBytes)
{
}

bool MySynthesizerProcessor::acceptsMidi() const
{
    return true;
}

//==============================================================================
const juce::String MySynthesizerProcessor::getName() const { return JucePlugin_Name; }
bool MySynthesizerProcessor::producesMidi() const { return false; }
bool MySynthesizerProcessor::isMidiEffect() const { return false; }
double MySynthesizerProcessor::getTailLengthSeconds() const { return 0.0; }
int MySynthesizerProcessor::getNumPrograms() { return 1; }
int MySynthesizerProcessor::getCurrentProgram() { return 0; }
void MySynthesizerProcessor::setCurrentProgram(int) { }
const juce::String MySynthesizerProcessor::getProgramName(int) { return {}; }
void MySynthesizerProcessor::changeProgramName(int, const juce::String&) { }
bool MySynthesizerProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* MySynthesizerProcessor::createEditor() { return new MySynthesizerProcessorEditor(*this); }
//==============================================================================

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MySynthesizerProcessor();
}