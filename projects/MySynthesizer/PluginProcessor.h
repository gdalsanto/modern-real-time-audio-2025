#pragma once

#include <JuceHeader.h>
#include "EnvelopeGenerator.h"
#include "SynthVoice.h"
#include "StateVariableFilter.h"
#include "mrta_utils/Source/Parameter/ParameterManager.h"

namespace Param
{

    namespace ID
    {
        // Filter parameters
        static const juce::String Freq { "freq" };
        static const juce::String Reso { "reso" };
        static const juce::String Mode { "mode" };
        // Envelope parameters
        static const juce::String Trigger { "gate" };
        static const juce::String AttTime { "att_time" };
        static const juce::String DecTime { "dec_time" };
        static const juce::String SusLevel { "sus_level" };
        static const juce::String RelTime { "rel_time" };
        // Source parameters
        static const juce::String WaveType { "wave_type" };
    }

    namespace Name
    {
        // Filter parameters
        static const juce::String Freq { "Frequency" };
        static const juce::String Reso { "Resonance" };
        static const juce::String Mode { "Mode" };
        // Envelope parameters
        static const juce::String Trigger { "Gate" };
        static const juce::String AttTime { "Att. Time" };
        static const juce::String DecTime { "Dec. Time" };
        static const juce::String SusLevel { "Sus. Level" };
        static const juce::String RelTime { "Rel. Time" };
        // Source parameters
        static const juce::String WaveType { "Wave Type" };
    }

    namespace Ranges
    {
        // Filter parameters
        static constexpr float FreqMin { 100.f };
        static constexpr float FreqMax { 10000.f };
        static constexpr float FreqInc { 1.f };
        static constexpr float FreqSkw { 0.4f };

        static constexpr float ResoMin { 0.5f };
        static constexpr float ResoMax { 5.f };
        static constexpr float ResoInc { 0.01f };
        static constexpr float ResoSkw { 0.4f };

        static constexpr float ModeMin { -1.f };
        static constexpr float ModeMax { 1.f };
        static constexpr float ModeInc { 0.01f };
        static constexpr float ModeSkw { 1.f };       
        
        // Envelope parameters
        static const juce::String TriggerOn { "ON" };
        static const juce::String TriggerOff { "OFF" };

        static constexpr float AttTimeMin { 1.f };
        static constexpr float AttTimeMax { 1000.f };
        static constexpr float AttTimeInc { 0.1f };
        static constexpr float AttTimeSkw { 0.5f };
    
        static constexpr float DecTimeMin { 1.f };
        static constexpr float DecTimeMax { 1000.f };
        static constexpr float DecTimeInc { 0.1f };
        static constexpr float DecTimeSkw { 0.5f };

        static constexpr float SusMin { 0.f };
        static constexpr float SusMax { 1.f };
        static constexpr float SusInc { 0.01f };
        static constexpr float SusSkw { 1.f };

        static constexpr float RelTimeMin { 1.f };
        static constexpr float RelTimeMax { 1000.f };
        static constexpr float RelTimeInc { 0.1f };
        static constexpr float RelTimeSkw { 0.5f };

        // Source parameters        '
        static const juce::StringArray WaveType { "Triangular", "Sawtooth" };
    }
}

class MySynthesizerProcessor : public juce::AudioProcessor
{
public:
    MySynthesizerProcessor();
    ~MySynthesizerProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void releaseResources() override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    mrta::ParameterManager& getParamManager() { return paramManager; }

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;
    //==============================================================================

private:
    mrta::ParameterManager paramManager;
    juce::Synthesiser synth;
    DSP::SynthVoice* voice { nullptr };
    DSP::EnvelopeGenerator env;
    DSP::StateVariableFilter SVF;
    // SVF parameters
    float freqHz { 1000.f };
    float freqModAmt { 0.f };
    float reso { 0.7071f };
    float mode { 0.5f };

    DSP::Ramp<float> freqRamp;
    DSP::Ramp<float> resoRamp;
    DSP::Ramp<float> lpfRamp;
    DSP::Ramp<float> bpfRamp;
    DSP::Ramp<float> hpfRamp;

    juce::AudioBuffer<float> freqInBuffer;
    juce::AudioBuffer<float> resoInBuffer;
    juce::AudioBuffer<float> lpfOutBuffer;
    juce::AudioBuffer<float> bpfOutBuffer;
    juce::AudioBuffer<float> hpfOutBuffer;

    static constexpr float FreqModAmtMax { 0.5f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MySynthesizerProcessor)
};