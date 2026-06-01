/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include"../Source/Parametrs/Parametrs.h"

//==============================================================================
/**
*/
class VKRprojectAudioProcessor  : public juce::AudioProcessor,
                                  public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    VKRprojectAudioProcessor();
    ~VKRprojectAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Processing settings
    void setFrequencyProcessing(bool newValue) { isFrequencyProcessing = newValue; }
    void setDelayProcessing(bool newValue) { isDelayProcessing = newValue; }
    void setReverbProcessing(bool newValue) { isReverbProcessing = newValue; }
    void setDynamicProcessing(bool newValue) { isDynamicProcessing = newValue; }

    // Frequency
    float lastSampleRate;
    juce::String filterType = "low-pass";

    // Delay effects
    juce::String lowFuncType = "atan(sin(x))";

    void setEnabledChorus(bool newValue) { enabledChorus = newValue; }
    void setEnabledFeedback(bool newValue) { enabledFeedback = newValue; }

    // Reverb
    juce::File root, savedFile;
    juce::dsp::Convolution irLoader;

    juce::ValueTree variableTree;
    juce::AudioProcessorValueTreeState treeState;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParametrLayout();
    void parameterChanged(const juce::String& parametrID, float newValue) override;
    void updateParameters();

    // Processing settings
    bool isFrequencyProcessing = false;
    bool isDelayProcessing = false;
    bool isReverbProcessing = false;
    bool isDynamicProcessing = false;

    // Frequency
    juce::dsp::ProcessorDuplicator<juce::dsp::StateVariableFilter::Filter<float>, juce::dsp::StateVariableFilter::Parameters<float>> stateVariableFilter;

    // Delay effects
    juce::AudioBuffer<float> DelayBuffer;
    int WritePosition{ 0 };
    int lowFuncVar = 0;

    float DalayTime;
    float Balance;
    float Width;

    bool enabledChorus = false;
    bool enabledFeedback = false;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VKRprojectAudioProcessor)
};
