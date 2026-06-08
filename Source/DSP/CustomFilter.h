/*
  ==============================================================================

    CustomFilter.h
    Created: 8 Jun 2026 5:32:48pm
    Author:  irodi

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>

class NOrderButterworth
{
public:
    enum class FilterType
    {
        LowPass,
        HighPass
    };

    NOrderButterworth() = default;

    void init(int order)
    {
        filterOrder = juce::jmax(1, order);

        int numBiquads = (filterOrder + 1) / 2;
        stages.resize(numBiquads);

        reset();
    }

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        reset();
        updateCoefficients();
    }

    void reset()
    {
        for (auto& stage : stages)
            stage.reset();
    }

    void setParameters(FilterType type, float cutoffFrequency)
    {
        currentType = type;

        float maxCutoff = currentSampleRate > 0.0 ? static_cast<float>(currentSampleRate) * 0.49f : 20000.0f;
        targetCutoff = juce::jlimit(20.0f, maxCutoff, cutoffFrequency);

        updateCoefficients();
    }

    forcedinline float processSample(float inputSample)
    {
        float output = inputSample;

        for (auto& stage : stages)
        {
            output = stage.process(output);
        }

        return output;
    }

private:
    struct BiquadStage
    {
        float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
        float a1 = 0.0f, a2 = 0.0f;

        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;

        bool isFirstOrder = false;

        void reset()
        {
            x1 = x2 = y1 = y2 = 0.0f;
        }

        forcedinline float process(float in)
        {
            float out = 0.0f;

            if (isFirstOrder)
            {
                out = (b0 * in) + (b1 * x1) - (a1 * y1);
            }
            else
            {
                out = (b0 * in) + (b1 * x1) + (b2 * x2) - (a1 * y1) - (a2 * y2);
            }

            x2 = x1;
            x1 = in;
            y2 = y1;
            y1 = out;

            return out;
        }
    };

    void updateCoefficients()
    {
        if (currentSampleRate <= 0.0 || stages.empty()) return;

        float theta = juce::MathConstants<float>::pi * targetCutoff / static_cast<float>(currentSampleRate);
        float tanTheta = std::tan(theta);

        int numBiquads = static_cast<int>(stages.size());

        for (int i = 0; i < numBiquads; ++i)
        {
            if ((filterOrder % 2 != 0) && (i == numBiquads - 1))
            {
                stages[i].isFirstOrder = true;

                if (currentType == FilterType::LowPass)
                {
                    stages[i].a1 = (tanTheta - 1.0f) / (tanTheta + 1.0f);
                    stages[i].b0 = tanTheta / (tanTheta + 1.0f);
                    stages[i].b1 = stages[i].b0;
                }
                else
                {
                    stages[i].a1 = (tanTheta - 1.0f) / (tanTheta + 1.0f);
                    stages[i].b0 = 1.0f / (tanTheta + 1.0f);
                    stages[i].b1 = -stages[i].b0;
                }
            }
            else
            {
                stages[i].isFirstOrder = false;
                int k = i + 1;
                float poleAngle = juce::MathConstants<float>::pi * static_cast<float>(2 * k + filterOrder - 1) / static_cast<float>(2 * filterOrder);
                float q = 1.0f / (-2.0f * std::cos(poleAngle));

                float K = tanTheta;
                float norm = 1.0f / (1.0f + K / q + K * K);

                if (currentType == FilterType::LowPass)
                {
                    stages[i].b0 = K * K * norm;
                    stages[i].b1 = 2.0f * stages[i].b0;
                    stages[i].b2 = stages[i].b0;
                }
                else // HighPass
                {
                    stages[i].b0 = norm;
                    stages[i].b1 = -2.0f * stages[i].b0;
                    stages[i].b2 = stages[i].b0;
                }

                stages[i].a1 = 2.0f * (K * K - 1.0f) * norm;
                stages[i].a2 = (1.0f - K / q + K * K) * norm;
            }
        }
    }

    int filterOrder = 4;
    double currentSampleRate = 44100.0;
    FilterType currentType = FilterType::LowPass;
    float targetCutoff = 1000.0f;

    std::vector<BiquadStage> stages;
};