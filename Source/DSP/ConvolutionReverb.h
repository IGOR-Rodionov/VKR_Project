/*
  ==============================================================================

    ConvolutionReverb.h
    Created: 14 Apr 2026 4:20:32pm
    Author:  irodi

  ==============================================================================
*/

#pragma once

class CustomConvolution
{
public:
    CustomConvolution()
    {
        formatManager.registerBasicFormats();
    }

    // Loading Impulse Response file 
    bool loadImpulseResponse(const juce::File& irFile)
    {
        // Check file existence
        if (!irFile.existsAsFile())
            return false;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(irFile));

        // Reading faile data
        if (reader != nullptr)
        {
            juce::AudioBuffer<float> tempBuffer;
            tempBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);

            // Read data from file
            reader->read(&tempBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

            // Update class data
            const juce::ScopedLock sl(processLock);
            impulseResponse = tempBuffer;
            irLength = impulseResponse.getNumSamples();
            updateHistoryBuffers();
            return true;
        }

        return false;
    }

    // Prepare reverb before processing
    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        const juce::ScopedLock sl(processLock);

        sampleRate = spec.sampleRate;
        maxBlockSize = spec.maximumBlockSize;
        numChannels = spec.numChannels;

        updateHistoryBuffers();
    }

    // Processing the signal
    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        const juce::ScopedTryLock sl(processLock);

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        const int numSamples = (int)inputBlock.getNumSamples();
        const int numCh = (int)inputBlock.getNumChannels();

        // Check ñorrectness of impulse response
        if (irLength == 0 || !sl.isLocked())
            return;

        for (int ch = 0; ch < numCh; ++ch)
        {
            // Get all data
            auto* input = inputBlock.getChannelPointer(ch);
            auto* output = outputBlock.getChannelPointer(ch);

            // Protection in case IR has less channels in IR than siganl
            auto* ir = impulseResponse.getReadPointer(juce::jmin(ch, impulseResponse.getNumChannels() - 1));
            auto* history = historyBuffers.getWritePointer(ch);
            int writeIdx = writeIndices[ch];

            // Convolution
            for (int i = 0; i < numSamples; ++i)
            {
                history[writeIdx] = input[i];

                float outSample = 0.0f;
                int readIdx = writeIdx;

                // Calculate new value
                for (int j = 0; j < irLength; ++j)
                {
                    outSample += history[readIdx] * ir[j];

                    readIdx--;
                    if (readIdx < 0)
                        readIdx = irLength - 1;
                }

                output[i] = outSample;

                writeIdx++;
                if (writeIdx >= irLength)
                    writeIdx = 0;
            }

            writeIndices[ch] = writeIdx;
        }
    }

    void reset()
    {
        const juce::ScopedLock sl(processLock);
        historyBuffers.clear();
        for (int ch = 0; ch < (int)numChannels; ++ch)
            writeIndices[ch] = 0;
    }

private:
    double sampleRate = 0.0;
    juce::uint32 maxBlockSize = 0;
    juce::uint32 numChannels = 0;

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> impulseResponse;
    int irLength = 0;

    juce::AudioBuffer<float> historyBuffers;
    std::vector<int> writeIndices;

    juce::CriticalSection processLock; // For thread safe file upload

    void updateHistoryBuffers()
    {
        if (numChannels > 0 && irLength > 0)
        {
            historyBuffers.setSize((int)numChannels, irLength);
            historyBuffers.clear();

            writeIndices.resize(numChannels);
            std::fill(writeIndices.begin(), writeIndices.end(), 0);
        }
    }
};