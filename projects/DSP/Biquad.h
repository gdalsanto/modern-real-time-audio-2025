#pragma once

#include <array>
#include <vector>

namespace DSP
{

class Biquad
{
public:
    Biquad(unsigned int numSections, unsigned int maxNumChannels);
    Biquad();
    ~Biquad();

    // this time we want to keep the default copy constructor
    Biquad(const Biquad&);  // so that we are able to create another Biquad instance from an existing one (Biquad a(b))
    const Biquad& operator=(const Biquad&); // make the assignment operator so that we can assign one Biquad to another ( a == b)

    Biquad(Biquad&&) = delete;
    const Biquad& operator=(Biquad&&) = delete;

    static const unsigned int CoeffsPerSection = 5;
    static const unsigned int StatesPerSection = 4;

    // Clear all states - good idea if there's a big change (e.g. change in type of filter)
    void clear();

    // Reallocate state storage
    // Calling this method will clear the states
    void reallocateChannels(unsigned int maxNumChannels);

    // Reallocate coefficient and state storage
    // Calling this method will clear the coefficients and states
    void reallocateSections(unsigned int numSections);

    // Set new coeffs to just the chosen section
    void setSectionCoeffs(const std::array<float, CoeffsPerSection>& newSectionCoeffs, unsigned int section);

    // Process audio
    // This method can be called with a lower number of channels than allocated
    void process(float* const* output, const float* const* input, unsigned int numChannels, unsigned int numSamples);

    // Process audio
    // Single sample flavour
    // output and input must have only one sample per channel
    void process(float* output, const float* input, unsigned int numChannels);

    // return the number of currently allocated channels
    unsigned int getAllocatedChannels() const noexcept { return allocatedChannels; }

    // return the number of currently allocated sections
    unsigned int getAllocatedSections() const noexcept { return allocatedSections; }

private:
    unsigned int allocatedChannels { 0 };
    unsigned int allocatedSections { 0 };

    // vector of coeffs of all sections
    // [sos0_b0, sos0_b1, sos0_b2, sos0_a1, sos0_a2, sos1_b0, sos1_b1, ...]
    // flat array with all the coefficients
    std::vector<float> coeffs;

    // vector of states of all channels and sections
    // [ch0_sos0_bz1, ... , ch0_sos0_az2, ch0_sos1_bz1, ... , ch0_sos1_az2, ... ,
    //  ch1_sos0_bz1, ... , ch1_sos0_az2, ch1_sos1_bz1, ... , ch1_sos1_az2, ...]
    std::vector<float> states;
};

}