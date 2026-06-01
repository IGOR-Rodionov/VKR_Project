/*
  ==============================================================================

    LVCompressor.h
    Created: 14 Apr 2026 4:08:12pm
    Author:  irodi

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class LVCompressor
{
public:
    LVCompressor();

    void prepare(juce::dsp::ProcessSpec& spec) noexcept;
    void process(juce::AudioBuffer<float>& buffer) noexcept;

    void setThreshold(float newThresh);
    void setRatio(float newRatio);
    void setAttack(float newAttack);
    void setRelease(float newRelease);
    void setMix(float newMix);

private:
    float _thresh;
    float _ratio;
    float _attack;
    float _release;

    juce::SmoothedValue<float> _mix;

    float _samplerate = 44100.f;
    float gainSC = 0.f;
    float gainSmooth = 0.f;
    float gainSmoothPrevious = 0.f;
    float currentSignal = 0.f;
    float gainChange_dB = 0.f;

    float processSample(float input);
};