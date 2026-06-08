/*
  ==============================================================================

    ConvolutionReverb.h
    Created: 8 Jun 2026 5:32:32pm
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

    bool loadImpulseResponse(const juce::File& irFile)
    {
        if (!irFile.existsAsFile())
            return false;

        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(irFile));

        if (reader != nullptr)
        {
            juce::AudioBuffer<float> tempBuffer;
            tempBuffer.setSize((int)reader->numChannels, (int)reader->lengthInSamples);

            reader->read(&tempBuffer, 0, (int)reader->lengthInSamples, 0, true, true);

            const juce::ScopedLock sl(processLock);
            impulseResponse = tempBuffer;
            irLength = impulseResponse.getNumSamples();

            updateHistoryBuffers();
            return true;
        }

        return false;
    }

    void prepare(const juce::dsp::ProcessSpec& spec)
    {
        const juce::ScopedLock sl(processLock);

        sampleRate = spec.sampleRate;
        maxBlockSize = spec.maximumBlockSize;
        numChannels = spec.numChannels;

        updateHistoryBuffers();
    }

    void process(const juce::dsp::ProcessContextReplacing<float>& context)
    {
        const juce::ScopedTryLock sl(processLock);

        const auto& inputBlock = context.getInputBlock();
        auto& outputBlock = context.getOutputBlock();

        const int numSamples = (int)inputBlock.getNumSamples();
        const int numCh = (int)inputBlock.getNumChannels();

        if (irLength == 0 || !sl.isLocked())
            return;

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* input = inputBlock.getChannelPointer(ch);
            auto* output = outputBlock.getChannelPointer(ch);

            auto* ir = impulseResponse.getReadPointer(juce::jmin(ch, impulseResponse.getNumChannels() - 1));
            auto* history = historyBuffers.getWritePointer(ch);
            int writeIdx = writeIndices[ch];

            for (int i = 0; i < numSamples; ++i)
            {
                history[writeIdx] = input[i];

                float outSample = 0.0f;
                int readIdx = writeIdx;

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

    double sampleRate = 0.0;
    juce::uint32 maxBlockSize = 0;
    juce::uint32 numChannels = 0;

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> impulseResponse;
    int irLength = 0;

    juce::AudioBuffer<float> historyBuffers;
    std::vector<int> writeIndices;

    juce::CriticalSection processLock; // Для потокобезопасной загрузки файлов
};