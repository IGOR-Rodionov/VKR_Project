/*
  ==============================================================================

    GUIprops.cpp
    Created: 2 May 2026 12:52:37pm
    Author:  irodi

  ==============================================================================
*/

#include "../Source/PluginEditor.h"

void VKRprojectAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    // Processing settings
    if (button == &frequencyButton)
    {
        audioProcessor.setFrequencyProcessing(true);
        audioProcessor.setDelayProcessing(false);
        audioProcessor.setReverbProcessing(false);
        audioProcessor.setDynamicProcessing(false);
        return;
    }
    if (button == &delayEffectsButton)
    {
        audioProcessor.setFrequencyProcessing(false);
        audioProcessor.setDelayProcessing(true);
        audioProcessor.setReverbProcessing(false);
        audioProcessor.setDynamicProcessing(false);
        return;
    }
    if (button == &reverbButton)
    {
        audioProcessor.setFrequencyProcessing(false);
        audioProcessor.setDelayProcessing(false);
        audioProcessor.setReverbProcessing(true);
        audioProcessor.setDynamicProcessing(false);
        return;
    }
    if (button == &dynamicButton)
    {
        audioProcessor.setFrequencyProcessing(false);
        audioProcessor.setDelayProcessing(false);
        audioProcessor.setReverbProcessing(false);
        audioProcessor.setDynamicProcessing(true);
        return;
    }
    // Delay
    if (button == &chorusButton)
    {
        if (chorusButton.getToggleState())
        {
            //chorusButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setEnabledChorus(true);
            return;
        }
        else
        {
            //chorusButton.setToggleState(true, juce::dontSendNotification);
            audioProcessor.setEnabledChorus(false);
            return;
        }
    }
    if (button == &feedbackButton)
    {
        if (feedbackButton.getToggleState())
        {
            //feedbackButton.setToggleState(false, juce::dontSendNotification);
            audioProcessor.setEnabledFeedback(true);
            return;
        }
        else
        {
            //feedbackButton.setToggleState(true, juce::dontSendNotification);
            audioProcessor.setEnabledFeedback(false);
            return;
        }
    }
}

void VKRprojectAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    // Frequency
    if (comboBoxThatHasChanged == &filterTypeBox)
    {
        audioProcessor.filterType = filterTypeBox.getItemText(filterTypeBox.getSelectedId());
    }
    // Delay
    if (comboBoxThatHasChanged == &lowFuncTypeBox)
    {
        audioProcessor.lowFuncType = lowFuncTypeBox.getItemText(lowFuncTypeBox.getSelectedId());
    }
}


void VKRprojectAudioProcessorEditor::setCommonSliderProps(juce::Slider& slider)
{
    addAndMakeVisible(slider);

    // Style
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 140, 36);
    slider.setLookAndFeel(&customDialAF);

    // Colors
    slider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, juce::Colours::transparentBlack);
    slider.setColour(juce::Slider::ColourIds::backgroundColourId, juce::Colours::whitesmoke.darker(0.8f));
    slider.setColour(juce::Slider::ColourIds::rotarySliderOutlineColourId, juce::Colours::black.brighter(0.25));
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour::fromRGB(251, 242, 252).darker(0.15));
    slider.setColour(juce::Slider::ColourIds::thumbColourId, juce::Colour::fromRGBA(0.392f, 0.584f, 0.929f, 1.f).darker(1.f));

    // Shadow
    shadowProperties.radius = 24;
    shadowProperties.offset = juce::Point<int>(0, 0);
    shadowProperties.colour = juce::Colours::black;
    SliderShadow.setShadowProperties(shadowProperties);
    slider.setComponentEffect(&SliderShadow);
}

void VKRprojectAudioProcessorEditor::setCommonLabelProps(juce::Label& label)
{
    addAndMakeVisible(label);
    label.setFont(juce::Font("Helvetica", 16.f, juce::Font::FontStyleFlags::bold));
    label.setJustificationType(juce::Justification::centred);
}

void VKRprojectAudioProcessorEditor::setGroupProps(juce::GroupComponent& group)
{
    addAndMakeVisible(group);
}

void VKRprojectAudioProcessorEditor::setCommonButtonProps(juce::ToggleButton& button)
{
    addAndMakeVisible(button);
    button.changeWidthToFitText();
    //button.setRadioGroupId(123);
    button.setClickingTogglesState(true);
}

void VKRprojectAudioProcessorEditor::attachSliders()
{
    // Frequency processing
    frequencyAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, cutoffFrequencyID, frequencySlider);
    resonanceAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, resonanceId, resonanceSlider);

    // Spatial processing
    delayAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, delayTimeID, DelaySlider);
    balanceAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, balanceID, BalanceSlider);
    widthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, widthID, WidthSlider);

    // Dynamic processing
    inputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, inputID, inputSlider);
    threshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, threshID, threshSlider);
    ratioAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, ratioID, ratioSlider);
    attackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, attackID, attackSlider);
    releaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, releaseID, releaseSlider);
    compMixAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, compMixID, compMixSlider);
    limiterThreshAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, lThreshID, limiterThreshSlider);
    limiterReleaseAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, lReleaseID, limiterReleaseSlider);
    outputAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.treeState, outputID, outputSlider);
}