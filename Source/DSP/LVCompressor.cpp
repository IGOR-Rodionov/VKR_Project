/*
  ==============================================================================

    LVCompressor.cpp
    Created: 14 Apr 2026 4:08:12pm
    Author:  irodi

  ==============================================================================
*/

#include "LVCompressor.h"

LVCompressor::LVCompressor()
{
}

void LVCompressor::prepare(juce::dsp::ProcessSpec& spec) noexcept
{
    _mix.reset(spec.sampleRate, 0.02);
    _samplerate = spec.sampleRate;
}

void LVCompressor::process(juce::AudioBuffer<float>& buffer) noexcept
{
    auto data = buffer.getArrayOfWritePointers();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)    
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            data[ch][sample] = processSample(data[ch][sample]);
        }
    }
}

float LVCompressor::processSample(float input)
{
    // Attack and release variable
    auto alphaAttack = std::exp((std::log(9) * -1.) / (_samplerate * _attack));
    auto alphaRelease = std::exp((std::log(9) * -1.) / (_samplerate * _release));

    const auto x = input;

    // Turn input unipolor on the dB scale
    auto x_Uni = abs(x);
    auto x_dB = juce::Decibels::gainToDecibels(x_Uni);

    // Avoid negative infinite number
    if (x_dB < -96.)
    {
        x_dB = -96.;
    }

    if (x_dB > _thresh)
    {
        gainSC = _thresh + (x_dB - _thresh) / _ratio;
    }
    else
    {
        gainSC = x_dB;
    }

    gainChange_dB = gainSC - x_dB;
    if (gainChange_dB < gainSmoothPrevious)
    {
        gainSmooth = ((1 - alphaAttack) * gainChange_dB) + (alphaAttack * gainSmoothPrevious);
        currentSignal = gainSmooth;
    }
    else
    {
        gainSmooth = ((1 - alphaRelease) * gainChange_dB) + (alphaRelease * gainSmoothPrevious);
        currentSignal = gainSmooth;
    }

    gainSmoothPrevious = gainSmooth;
    auto wetInput = x * juce::Decibels::decibelsToGain(gainSmooth);
    auto mix = (1. - _mix.getNextValue()) * x + wetInput * _mix.getNextValue();
    return mix;
}

void LVCompressor::setThreshold(float newThresh)
{
    _thresh = newThresh;
}

void LVCompressor::setRatio(float newRatio)
{
    _ratio = newRatio;
}

void LVCompressor::setAttack(float newAttack)
{
    _attack = newAttack / 1000.f;
}

void LVCompressor::setRelease(float newRelease)
{
    _release = newRelease / 1000.f;
}

void LVCompressor::setMix(float newMix)
{
    _mix.setTargetValue(newMix);
}
