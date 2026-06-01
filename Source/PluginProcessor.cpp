/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VKRprojectAudioProcessor::VKRprojectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , treeState(*this, nullptr, "PARAMETRS", createParametrLayout())
#endif
{
    // Frequency processing
    treeState.addParameterListener(cutoffFrequencyID, this);
    treeState.addParameterListener(resonanceId, this);

    // Spatial processing
    treeState.addParameterListener(delayTimeID, this);
    treeState.addParameterListener(balanceID, this);
    treeState.addParameterListener(widthID, this);

    // Reverb
    variableTree =
    {
        "Variables", {},
        {
            {"Group",{{"name","IR Vars"}},
                {
                    {"Parameter",{{"id","file1"},{"value","/"}}},
                        {"Parameter",{{"id","root"},{"value","/"}}}
                }
            }
        }
    };

    // Dynamic processing
    treeState.addParameterListener(inputID, this);
    treeState.addParameterListener(threshID, this);
    treeState.addParameterListener(ratioID, this);
    treeState.addParameterListener(attackID, this);
    treeState.addParameterListener(releaseID, this);
    treeState.addParameterListener(compMixID, this);
    treeState.addParameterListener(lThreshID, this);
    treeState.addParameterListener(lReleaseID, this);
    treeState.addParameterListener(outputID, this);
}

VKRprojectAudioProcessor::~VKRprojectAudioProcessor()
{
    // Frequency processing
    treeState.removeParameterListener(cutoffFrequencyID, this);
    treeState.removeParameterListener(resonanceId, this);

    // Spatial processing
    treeState.removeParameterListener(delayTimeID, this);
    treeState.removeParameterListener(balanceID, this);
    treeState.removeParameterListener(widthID, this);

    // Dynamic processing
    treeState.removeParameterListener(inputID, this);
    treeState.removeParameterListener(threshID, this);
    treeState.removeParameterListener(ratioID, this);
    treeState.removeParameterListener(attackID, this);
    treeState.removeParameterListener(releaseID, this);
    treeState.removeParameterListener(compMixID, this);
    treeState.removeParameterListener(lThreshID, this);
    treeState.removeParameterListener(lReleaseID, this);
    treeState.removeParameterListener(outputID, this);
}


juce::AudioProcessorValueTreeState::ParameterLayout VKRprojectAudioProcessor::createParametrLayout()
{
    /* Set Slider's range */
    // Frequency processing
    juce::NormalisableRange<float> cutOffFrequencyRange = juce::NormalisableRange<float>(20.f, 22000.f, 10.f);
    cutOffFrequencyRange.setSkewForCentre(3500.f);
    juce::NormalisableRange<float> resonanceRange = juce::NormalisableRange<float>(0.f, 5.f, 0.05f);

    // Spatial processing
    juce::NormalisableRange<float> delayTimeRange = juce::NormalisableRange<float>(0.f, 2000.f, 10.f);
    juce::NormalisableRange<float> balanceRange = juce::NormalisableRange<float>(0.f, 100.f, 0.05f);
    juce::NormalisableRange<float> widthRange = juce::NormalisableRange<float>(0.f, 5.f, 0.01f);

    // Dynamic processing
    juce::NormalisableRange<float> attackRange = juce::NormalisableRange<float>(0.f, 200.f, 1.f);
    attackRange.setSkewForCentre(50.f);
    juce::NormalisableRange<float> releaseRange = juce::NormalisableRange<float>(5.f, 5000.f, 1.f);
    releaseRange.setSkewForCentre(160.f);
    juce::NormalisableRange<float> lReleaseRange = juce::NormalisableRange<float>(1.f, 1000.f, 1.f);
    lReleaseRange.setSkewForCentre(250.f);

    /* Set parametrs */
    // Frequency processing
    auto pCuttOffFrequency = std::make_unique<juce::AudioParameterFloat>(cutoffFrequencyID, cutoffFrequencyName, cutOffFrequencyRange, 200.f);
    auto pResonance = std::make_unique<juce::AudioParameterFloat>(resonanceId, resonanceName, resonanceRange, 50.f);

    // Spatial processing
    auto pDelayTime = std::make_unique<juce::AudioParameterFloat>(delayTimeID, delayTimeName, delayTimeRange, 200.f);
    auto pBalance = std::make_unique<juce::AudioParameterFloat>(balanceID, balanceName, balanceRange, 50.f);
    auto pWidth = std::make_unique<juce::AudioParameterFloat>(widthID, widthName, widthRange, 0.f);

    // Dynamic processing
    auto pInput = std::make_unique<juce::AudioParameterFloat>(inputID, inputName, -60.f, 24.f, 0.f);
    auto pThresh = std::make_unique<juce::AudioParameterFloat>(threshID, threshName, -60.f, 10.f, 0.f);
    auto pRatio = std::make_unique<juce::AudioParameterFloat>(ratioID, ratioName, 1.f, 20.f, 1.f);
    auto pAttack = std::make_unique<juce::AudioParameterFloat>(attackID, attackName, attackRange, 50.f);
    auto pRelease = std::make_unique<juce::AudioParameterFloat>(releaseID, releaseName, releaseRange, 160.f);
    auto pCompMix = std::make_unique<juce::AudioParameterFloat>(compMixID, compMixName, 0.f, 1.f, 1.f);
    auto pLThresh = std::make_unique<juce::AudioParameterFloat>(lThreshID, lThreshName, -60.f, 0.f, 0.f);
    auto pLRelease = std::make_unique<juce::AudioParameterFloat>(lReleaseID, lReleaseName, lReleaseRange, 250.f);
    auto pOutput = std::make_unique<juce::AudioParameterFloat>(outputID, outputName, -60.f, 24.f, 0.f);

    /* Add parametrs */
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;

    // Frequency processing
    params.push_back(std::move(pCuttOffFrequency));
    params.push_back(std::move(pResonance));

    // Spatial processing
    params.push_back(std::move(pDelayTime));
    params.push_back(std::move(pBalance));
    params.push_back(std::move(pWidth));

    // Dynamic processing
    params.push_back(std::move(pInput));
    params.push_back(std::move(pThresh));
    params.push_back(std::move(pRatio));
    params.push_back(std::move(pAttack));
    params.push_back(std::move(pRelease));
    params.push_back(std::move(pCompMix));
    params.push_back(std::move(pLThresh));
    params.push_back(std::move(pLRelease));
    params.push_back(std::move(pOutput));


    return { params.begin(),params.end() };
}

void VKRprojectAudioProcessor::parameterChanged(const juce::String& parametrID, float newValue)
{
    updateParameters();
}

void VKRprojectAudioProcessor::updateParameters()
{
}

//==============================================================================
const juce::String VKRprojectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VKRprojectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VKRprojectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VKRprojectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VKRprojectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VKRprojectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VKRprojectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VKRprojectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VKRprojectAudioProcessor::getProgramName (int index)
{
    return {};
}

void VKRprojectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VKRprojectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VKRprojectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VKRprojectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VKRprojectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool VKRprojectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VKRprojectAudioProcessor::createEditor()
{
    return new VKRprojectAudioProcessorEditor (*this);
}

//==============================================================================
void VKRprojectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VKRprojectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VKRprojectAudioProcessor();
}
