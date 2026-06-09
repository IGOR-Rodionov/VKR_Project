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
    treeState.addParameterListener(orderId, this);

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
    treeState.removeParameterListener(orderId, this);

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
    juce::NormalisableRange<float> orderRange = juce::NormalisableRange<float>(1.f, 8.f, 1.f);

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
    auto porder = std::make_unique<juce::AudioParameterFloat>(orderId, orderName, orderRange, 50.f);

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
    params.push_back(std::move(porder));

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
    // Delay effects
    DalayTime = treeState.getRawParameterValue(delayTimeID)->load();
    Balance = treeState.getRawParameterValue(balanceID)->load() / 100;
    Width = treeState.getRawParameterValue(widthID)->load();

    // Frequency filters
    if (filterType == "high-pass")
    {
        stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::highPass;
        auto coefficientsArray = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(
            treeState.getRawParameterValue(cutoffFrequencyID)->load(),
            lastSampleRate,
            treeState.getRawParameterValue(orderId)->load()
        );

        if (treeState.getRawParameterValue(orderId)->load() == 2)
        {
            *filter2Chain.get<0>().coefficients = *coefficientsArray[0];
        }
        if (treeState.getRawParameterValue(orderId)->load() == 4)
        {
            *filter4Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter4Chain.get<1>().coefficients = *coefficientsArray[1];
        }
        if (treeState.getRawParameterValue(orderId)->load() == 6)
        {
            *filter6Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter6Chain.get<1>().coefficients = *coefficientsArray[1];
            *filter6Chain.get<2>().coefficients = *coefficientsArray[2];
        }
        if (treeState.getRawParameterValue(orderId)->load() == 8)
        {
            *filter8Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter8Chain.get<1>().coefficients = *coefficientsArray[1];
            *filter8Chain.get<2>().coefficients = *coefficientsArray[2];
            *filter8Chain.get<3>().coefficients = *coefficientsArray[3];
        }
    }
    else //if (filterType == "low-pass")
    {
        stateVariableFilter.state->type = juce::dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
        auto coefficientsArray = juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(
            treeState.getRawParameterValue(cutoffFrequencyID)->load(),
            lastSampleRate,
            treeState.getRawParameterValue(orderId)->load()
        );

        if (treeState.getRawParameterValue(orderId)->load() <= 2)
        {
            *filter2Chain.get<0>().coefficients = *coefficientsArray[0];
        }
        if (treeState.getRawParameterValue(orderId)->load() <= 4 &&
            treeState.getRawParameterValue(orderId)->load() > 2)
        {
            *filter4Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter4Chain.get<1>().coefficients = *coefficientsArray[1];
        }
        if (treeState.getRawParameterValue(orderId)->load() <= 6 &&
            treeState.getRawParameterValue(orderId)->load() > 4)
        {
            *filter6Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter6Chain.get<1>().coefficients = *coefficientsArray[1];
            *filter6Chain.get<2>().coefficients = *coefficientsArray[2];
        }
        if (treeState.getRawParameterValue(orderId)->load() <= 8 &&
            treeState.getRawParameterValue(orderId)->load() > 6)
        {
            *filter8Chain.get<0>().coefficients = *coefficientsArray[0];
            *filter8Chain.get<1>().coefficients = *coefficientsArray[1];
            *filter8Chain.get<2>().coefficients = *coefficientsArray[2];
            *filter8Chain.get<3>().coefficients = *coefficientsArray[3];
        }
    }

    // Compressor
    inputModule.setGainDecibels(treeState.getRawParameterValue(inputID)->load());

    lvCompressorModule.setThreshold(treeState.getRawParameterValue(threshID)->load());
    lvCompressorModule.setRatio(treeState.getRawParameterValue(ratioID)->load());
    lvCompressorModule.setAttack(treeState.getRawParameterValue(attackID)->load());
    lvCompressorModule.setRelease(treeState.getRawParameterValue(releaseID)->load());
    lvCompressorModule.setMix(treeState.getRawParameterValue(compMixID)->load());

    limiterModule.setThreshold(treeState.getRawParameterValue(lThreshID)->load());
    limiterModule.setRelease(treeState.getRawParameterValue(lReleaseID)->load());

    outputModule.setGainDecibels(treeState.getRawParameterValue(outputID)->load());
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
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    spec.maximumBlockSize = samplesPerBlock;
    spec.maximumBlockSize = getMainBusNumOutputChannels();

    // Frequency filters
    lastSampleRate = sampleRate;
    filter2Chain.reset();
    filter4Chain.reset();
    filter6Chain.reset();
    filter8Chain.reset();
    filter2Chain.prepare(spec);
    filter4Chain.prepare(spec);
    filter6Chain.prepare(spec);
    filter8Chain.prepare(spec);

    // IR Reverb
    Spec.maximumBlockSize = samplesPerBlock;
    Spec.sampleRate = sampleRate;
    Spec.numChannels = getTotalNumOutputChannels();

    irLoader.reset();
    irLoader.prepare(Spec);

    // Delay
    auto DelayBufferSize = sampleRate * 2.;
    DelayBuffer.setSize(getTotalNumOutputChannels(), (int)DelayBufferSize);
    DelayBuffer.clear();

    // Compressor
    inputModule.prepare(spec);
    inputModule.setRampDurationSeconds(0.02);
    outputModule.setRampDurationSeconds(0.02);
    outputModule.prepare(spec);
    lvCompressorModule.prepare(spec);
    limiterModule.prepare(spec);

    updateParameters();
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

    if (isReverbProcessing) 
    {
        juce::dsp::AudioBlock<float> block{ buffer };
        if (irLoader.getCurrentIRSize() > 0)
        {
            irLoader.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
    }
    if (isDelayProcessing) 
    {
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        {
            buffer.clear(i, 0, buffer.getNumSamples());
        }

        auto BufferSize = buffer.getNumSamples();
        auto DelayBufferSize = DelayBuffer.getNumSamples();

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            FillBuffer(channel, BufferSize, DelayBufferSize, channelData);
            ReadFromBuffer(buffer, DelayBuffer, channel, BufferSize, DelayBufferSize);
            if (enabledFeedback)
            {
                FillBuffer(channel, BufferSize, DelayBufferSize, channelData);
            }
        }

        lowFuncVar++;
        lowFuncVar %= 1000;
        WritePosition += BufferSize;
        WritePosition %= DelayBufferSize;
    }
    if (isFrequencyProcessing)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);

        updateParameters();
        if (treeState.getRawParameterValue(orderId)->load() <= 2)
        {
            filter2Chain.process(context);
        }
        if (treeState.getRawParameterValue(orderId)->load() <= 4 &&
            treeState.getRawParameterValue(orderId)->load() > 2)
        {
            filter4Chain.process(context);
        }
        if (treeState.getRawParameterValue(orderId)->load() <= 6 &&
            treeState.getRawParameterValue(orderId)->load() > 4)
        {
            filter6Chain.process(context);
        }

        if (treeState.getRawParameterValue(orderId)->load() <= 8 &&
            treeState.getRawParameterValue(orderId)->load() > 6)
        {
            filter8Chain.process(context);
        }
    }
    if (isDynamicProcessing) 
    {
        juce::dsp::AudioBlock<float> block{ buffer };

        inputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
        lvCompressorModule.process(buffer);
        limiterModule.process(juce::dsp::ProcessContextReplacing<float>(block));
        outputModule.process(juce::dsp::ProcessContextReplacing<float>(block));
    }
}

// Delay Effects
void VKRprojectAudioProcessor::FillBuffer(int channel, int BufferSize, int DelayBufferSize, float* channelData)
{
    if (DelayBufferSize > BufferSize + WritePosition)
    {
        DelayBuffer.copyFrom(channel, WritePosition, channelData, BufferSize);
    }
    else
    {
        auto NumSamplesToEnd = DelayBufferSize - WritePosition;
        DelayBuffer.copyFrom(channel, WritePosition, channelData, NumSamplesToEnd);

        auto NumSamplesAtStart = BufferSize - NumSamplesToEnd;
        DelayBuffer.copyFrom(channel, 0, channelData + NumSamplesToEnd, NumSamplesAtStart);
    }
}

void VKRprojectAudioProcessor::ReadFromBuffer(juce::AudioBuffer<float>& buffer, juce::AudioBuffer<float>& DelayBuffer, int channel, int BufferSize, int DelayBufferSize)
{
    auto ReadPosition = WritePosition - (int)(DalayTime * getSampleRate() / 1000);
    float Gain = Balance;
    if (enabledChorus)
    {
        Gain = Balance * lowFrequencyFunction();
    }

    if (ReadPosition < 0)
    {
        ReadPosition += DelayBufferSize;
    }
    if (ReadPosition + BufferSize < DelayBufferSize)
    {
        buffer.addFromWithRamp(channel, 0, DelayBuffer.getReadPointer(channel, ReadPosition), BufferSize, Gain, Gain);
    }
    else
    {
        auto NumSamplesToEnd = DelayBufferSize - ReadPosition;
        buffer.addFromWithRamp(channel, 0, DelayBuffer.getReadPointer(channel, ReadPosition), NumSamplesToEnd, Gain, Gain);

        auto NumSamplesAtStart = BufferSize - NumSamplesToEnd;
        buffer.addFromWithRamp(channel, NumSamplesToEnd, DelayBuffer.getReadPointer(channel, 0), NumSamplesAtStart, Gain, Gain);
    }
}

const double VKRprojectAudioProcessor::lowFrequencyFunction()
{
    if (lowFuncType == "sin(x)")
    {
        return std::sin(lowFuncVar * Width);
    }
    else if (lowFuncType == "tan(sin(x))")
    {
        return std::tan(std::sin(lowFuncVar) * Width);
    }
    else if (lowFuncType == "arctan(sin(x))")
    {
        return std::atan(std::sin(lowFuncVar) * Width);
    }
    else return 1.;
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
    treeState.state.appendChild(variableTree, nullptr);
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void VKRprojectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    variableTree = tree.getChildWithName("Variables");

    if (tree.isValid())
    {
        treeState.state = tree;

        savedFile = juce::File(variableTree.getProperty("file1"));
        root = juce::File(variableTree.getProperty("root"));

        if (savedFile.existsAsFile())
        {
            irLoader.loadImpulseResponse(savedFile,
                juce::dsp::Convolution::Stereo::yes,
                juce::dsp::Convolution::Trim::yes, 0);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VKRprojectAudioProcessor();
}