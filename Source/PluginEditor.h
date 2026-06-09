/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include"../Source/GUI/DialLookAtFeel.h"

//==============================================================================
/**
*/
class VKRprojectAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        public juce::ToggleButton::Listener,
                                        public juce::ComboBox::Listener
{
public:
    VKRprojectAudioProcessorEditor (VKRprojectAudioProcessor&);
    ~VKRprojectAudioProcessorEditor() override;

    // Listeners
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    /* Sliders */
    // Frequency
    juce::Slider frequencySlider;
    juce::Slider orderSlider;

    // Delay
    juce::Slider DelaySlider;
    juce::Slider BalanceSlider;
    juce::Slider WidthSlider;

    // Compressor
    juce::Slider inputSlider;
    juce::Slider outputSlider;

    juce::Slider threshSlider;
    juce::Slider ratioSlider;
    juce::Slider attackSlider;
    juce::Slider releaseSlider;
    juce::Slider compMixSlider;

    juce::Slider limiterThreshSlider;
    juce::Slider limiterReleaseSlider;

    std::vector<juce::Slider*> sliders =
    {
        &frequencySlider, &orderSlider,
        &DelaySlider, &BalanceSlider, &WidthSlider,
        &inputSlider,&threshSlider,&ratioSlider,&limiterThreshSlider,
        &outputSlider,&attackSlider,&releaseSlider,&limiterReleaseSlider,
        &compMixSlider
    };


    /* Labels */
    // Frequency
    juce::Label frequencyLabel;
    juce::Label orderLabel;

    // Delay
    juce::Label delayLabel;
    juce::Label balanceLabel;
    juce::Label widthLabel;

    // Reverb
    juce::Label irName;

    // Compressor
    juce::Label inputSliderLabel;
    juce::Label outputSliderLabel;

    juce::Label threshSliderLabel;
    juce::Label ratioSliderLabel;
    juce::Label attackSliderLabel;
    juce::Label releaseSliderLabel;
    juce::Label compMixSliderLabel;

    juce::Label limiterThreshSliderLabel;
    juce::Label limiterReleaseSliderLabel;

    std::vector<juce::Label*> slidersLabels =
    {
        &frequencyLabel, &orderLabel,
        &delayLabel, &balanceLabel, &widthLabel,
        &inputSliderLabel,&threshSliderLabel,&ratioSliderLabel,&limiterThreshSliderLabel,
        &outputSliderLabel,&attackSliderLabel,&releaseSliderLabel,&limiterReleaseSliderLabel,
        &compMixSliderLabel
    };

    // Button
    juce::TextButton loadBtn;
    std::unique_ptr<juce::FileChooser> fileChooser;

    /* Toggle buttons */
    // Prcessing type buttons
    juce::ToggleButton frequencyButton{ "Frequency processing" };
    juce::ToggleButton delayEffectsButton{ "Delay effects" };
    juce::ToggleButton reverbButton{ "IR Convolution" };
    juce::ToggleButton dynamicButton{ "Dynamic Processing" };

    // Delay
    juce::ToggleButton chorusButton{ "Chorus enabled" };
    juce::ToggleButton feedbackButton{ "Feedback enabled" };
    juce::ToggleButton disableButton{ "Disable all" };

    std::vector<juce::ToggleButton*> buttons =
    {
        &chorusButton, &feedbackButton, &disableButton,
        &frequencyButton, &delayEffectsButton, &reverbButton, &dynamicButton
    };

    // Comboboxes
    juce::ComboBox filterTypeBox;
    juce::ComboBox lowFuncTypeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> typeAttach;

    /* Groups*/
    // Prcessing type groups
    juce::GroupComponent processingSettingsGroup;
    juce::GroupComponent frequencyGroup;
    juce::GroupComponent delayEffectsGroup;
    juce::GroupComponent reverbGroup;
    juce::GroupComponent dynamicProcGroup;

    // Delay
    juce::GroupComponent delayGroup;
    juce::GroupComponent chorusGroup;

    // Compressor
    juce::GroupComponent ioGroup;
    juce::GroupComponent compressorGroup;
    juce::GroupComponent limiterGroup;

    std::vector<juce::GroupComponent*> groups =
    {
        &delayGroup, &chorusGroup,
        &ioGroup,&compressorGroup,&limiterGroup,
        &frequencyGroup, &delayEffectsGroup, &reverbGroup, &dynamicProcGroup,
        &processingSettingsGroup
    };

    // Visual
    dial::CustomDial customDialAF;
    juce::DropShadow shadowProperties;
    juce::DropShadowEffect SliderShadow;

    /* Slider attachment */
    // Frequency
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> orderAttach;

    // Delay effects
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> balanceAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttach;

    // Dynamics
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> threshAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ratioAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compMixAttach;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> limiterThreshAttach;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> limiterReleaseAttach;

    // Functions
    void setCommonSliderProps(juce::Slider& slider);
    void setCommonLabelProps(juce::Label& label);
    void setGroupProps(juce::GroupComponent& group);
    void setCommonButtonProps(juce::ToggleButton& button);
    void attachSliders();
    VKRprojectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VKRprojectAudioProcessorEditor)
};
