#include "Comb.h"

#include <cmath>

namespace DSP
{

Comb::Comb(float maxTimeMs, unsigned int numChannels) :
    // initialize delay line with max length in samples
    delayLine(static_cast<unsigned int>(std::ceil(std::fmax(maxTimeMs, 1.f) * static_cast<float>(0.001 * sampleRate))), static_cast<unsigned int>(std::ceil(std::fmax(numChannels, MaxChannels)))),
    // initialize the ramp time
    offsetRamp(0.05f),
    modDepthRamp(0.05f) 
{
}

Comb::~Comb()
{
}

void Comb::prepare(double newSampleRate, float maxTimeMs, unsigned int numChannels)
{
    sampleRate = newSampleRate;

    delayLine.prepare(static_cast<unsigned int>(std::round(maxTimeMs * static_cast<float>(0.001 * sampleRate))), static_cast<unsigned int>(std::ceil(std::fmax(numChannels, MaxChannels))));
    delayLine.setDelaySamples(static_cast<unsigned int>(std::ceil(0.001 * sampleRate))); // Set fixed delay to 1ms
    // skip ramps when preparing
    offsetRamp.prepare(sampleRate, true, offsetMs * static_cast<float>(0.001 * sampleRate));
    modDepthRamp.prepare(sampleRate, true, modDepthMs * static_cast<float>(0.001 * sampleRate));
    feedbackRamp.prepare(sampleRate, true, static_cast<float>(0.5f));

    phaseState[0] = 0.f;
    phaseState[1] = static_cast<float>(M_PI / 2.0);

    feedbackState[0] = 0.f;
    feedbackState[1] = 0.f;

    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void Comb::clear()
{
    delayLine.clear();
}

void Comb::process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples)
{
    for (unsigned int n = 0; n < numSamples; ++n)
    {
        // Process LFO according to modulation type
        float lfo[2] { 0.f, 0.f };
        switch (modType)
        {
        case Tri:
            lfo[0] = std::fabs((phaseState[0] - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            lfo[1] = std::fabs((phaseState[1] - static_cast<float>(M_PI)) / static_cast<float>(M_PI));
            break;

        case Sin:
            lfo[0] = 0.5f + 0.5f * std::sin(phaseState[0]);
            lfo[1] = 0.5f + 0.5f * std::sin(phaseState[1]);
            break;
        }

        // Increment and wrap phase states
        phaseState[0] = std::fmod(phaseState[0] + phaseInc, static_cast<float>(2 * M_PI));
        phaseState[1] = std::fmod(phaseState[1] + phaseInc, static_cast<float>(2 * M_PI));

        // Apply mod depth and offset ramps
        modDepthRamp.applyGain(lfo, numChannels);
        offsetRamp.applySum(lfo, numChannels);

        // Delay in/out
        float x[2];
        float y[2];

        for (unsigned int ch = 0; ch < numChannels; ++ch)
            x[ch] = input[ch][n] + feedbackState[ch];

        
        // Process delay
        delayLine.process(y, x, lfo, numChannels);

        // Write to output buffers
        for (unsigned int ch = 0; ch < numChannels; ++ch)
            output[ch][n] = y[ch];

        // apply feedback gain 
        feedbackRamp.applyGain(y, numChannels);
        for (unsigned int ch = 0; ch < 2; ++ch)
            // overwrite the value that will be fed back at the next iteration 
            feedbackState[ch] = y[ch];
            
    }
}

void Comb::setOffset(float newOffsetMs)
{
    // Since the fixed delay is set to 1ms
    // We can deduct that from the offset ramp
    offsetMs = std::fmax(newOffsetMs - 1.f, 0.f);
    offsetRamp.setTarget(offsetMs * static_cast<float>(0.001 * sampleRate));
}

void Comb::setDepth(float newDepthMs)
{
    modDepthMs = std::fmax(newDepthMs, 0.f);
    modDepthRamp.setTarget(modDepthMs * static_cast<float>(0.001 * sampleRate));
}

void Comb::setModulationRate(float newModRateHz)
{
    modRate = std::fmax(newModRateHz, 0.f);
    phaseInc = static_cast<float>(2.0 * M_PI / sampleRate) * modRate;
}

void Comb::setModulationType(ModulationType newModType)
{
    modType = newModType;
}

void Comb::setFeedbackGain(float newFeedbackGain)
{
    feedbackRamp.setTarget(newFeedbackGain);
}
}