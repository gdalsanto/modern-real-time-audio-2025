#pragma once

#include <JuceHeader.h>

#include "Oscillator.h"
#include "Ramp.h"

namespace DSP
{

float convertMidiNoteToFreq(int MidiNote);


// The SynthesiserSound is a passive class that just describes what the sound is -
// the actual audio rendering for a sound is done by a SynthesiserVoice. This allows
// more than one SynthesiserVoice to play the same sound at the same time.
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();
    ~SynthVoice();

    SynthVoice(const SynthVoice&) = delete;
    SynthVoice(SynthVoice&&) = delete;
    const SynthVoice& operator=(const SynthVoice&) = delete;
    const SynthVoice& operator=(SynthVoice&&) = delete;

    void setWaveType(Oscillator::OscType type);
    void setAttRelTime(float newAttRelTimeMs);

    // These methods have been already defined in the SynthesiserVoice class
    // so we need to override them to use our own implementation 
    bool canPlaySound(SynthesiserSound* ptr) override;  // in the library is "= 0 " so we must re-implement it
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound*, int currentPitchWheelPosition) override; 
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    double sampleRate { 48000.0 };

    Oscillator osc;
    Ramp<float> ramp;
};

}
