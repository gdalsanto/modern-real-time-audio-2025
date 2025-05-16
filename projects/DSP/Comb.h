#pragma once

#include "DelayLine.h"
#include "Ramp.h"

namespace DSP
{

class Comb
{
public:
    enum ModulationType : unsigned int
    {
        Sin = 0,
        Tri
    };

    Comb(float maxTimeMs, unsigned int numChannels);
    ~Comb();

    // No default ctor
    Comb() = delete;

    // No copy semantics
    Comb(const Comb&) = delete;
    const Comb& operator=(const Comb&) = delete;

    // No move semantics
    Comb(Comb&&) = delete;
    const Comb& operator=(Comb&&) = delete;

    // Update sample rate, reallocates and clear internal buffers
    void prepare(double sampleRate, float maxTimeMs, unsigned int numChannels);

    // Clear contents of internal buffer
    void clear();

    // Process audio
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Set delay offset in ms
    void setOffset(float newOffsetMs);

    // Set modulation depth in ms
    void setDepth(float newDepthMs);

    // Set delay time modulation rate in Hz
    void setModulationRate(float newModRateHz);

    // Set delay time modulation waveform type
    void setModulationType(ModulationType newModType);

    // Set feedback gain
    void setFeedbackGain(float newFeedbackGain);
    
    static constexpr int MaxChannels { 2 };

private:
    double sampleRate { 48000.0 };

    DSP::DelayLine delayLine;

    DSP::Ramp<float> offsetRamp;
    DSP::Ramp<float> modDepthRamp;
    DSP::Ramp<float> feedbackRamp;

    float feedbackState[2] { 0.f, 0.f };

    float phaseState[2] { 0.f, 0.f };
    float phaseInc { 0.f };

    float offsetMs { 0.f };
    float modDepthMs { 0.f };
    float modRate { 0.f };

    ModulationType modType { Sin };
};

}