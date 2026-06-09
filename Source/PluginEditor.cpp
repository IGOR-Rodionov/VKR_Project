/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VKRprojectAudioProcessorEditor::VKRprojectAudioProcessorEditor (VKRprojectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
    , frequencyLabel("Frequency", "Frequency")
    , orderLabel("Order", "Order")
    , delayLabel("Delay time", "Delay time")
    , balanceLabel("Balance", "Balance")
    , widthLabel("Width", "Width")
    , inputSliderLabel("Input", "Input")
    , threshSliderLabel("Thresh", "Thresh")
    , ratioSliderLabel("Ratio", "Ratio")
    , attackSliderLabel("Attack", "Attack")
    , releaseSliderLabel("Release", "Release")
    , compMixSliderLabel("Mix", "Mix")
    , limiterThreshSliderLabel("Thresh", "Thresh")
    , limiterReleaseSliderLabel("Release", "Release")
    , outputSliderLabel("Output", "Output")
{
    // Window
    setSize(1000, 675);
    juce::AudioProcessorEditor::setResizable(true, true);
    juce::AudioProcessorEditor::setResizeLimits(getWidth() * 0.75,
        getHeight() * 0.75, getWidth() * 1.75, getHeight() * 1.75);
    juce::AudioProcessorEditor::getConstrainer()->setFixedAspectRatio(1.49);

    // Arrays
    for (int i = 0; i < sliders.size(); i++)
    {
        setCommonSliderProps(*sliders[i]);
        setCommonLabelProps(*slidersLabels[i]);
        slidersLabels[i]->attachToComponent(sliders[i], false);
    }

    // Sliders
    frequencySlider.setTextValueSuffix(" Hz");
    DelaySlider.setTextValueSuffix(" ms");
    BalanceSlider.setTextValueSuffix(" %");
    WidthSlider.setTextValueSuffix(" Hz");
    attachSliders();

    /* Labels */
    // Frequency
    frequencyLabel.setText("Frequency", juce::NotificationType::dontSendNotification);
    orderLabel.setText("Order", juce::NotificationType::dontSendNotification);

    // Delay
    delayLabel.setText("Delay time", juce::NotificationType::dontSendNotification);
    balanceLabel.setText("Balance", juce::NotificationType::dontSendNotification);

    // Reverb
    addAndMakeVisible(irName);

    // Compressor
    inputSliderLabel.setText("Input", juce::NotificationType::dontSendNotification);
    outputSliderLabel.setText("Output", juce::NotificationType::dontSendNotification);

    threshSliderLabel.setText("Thresh", juce::NotificationType::dontSendNotification);
    ratioSliderLabel.setText("Ratio", juce::NotificationType::dontSendNotification);
    attackSliderLabel.setText("Attack", juce::NotificationType::dontSendNotification);
    releaseSliderLabel.setText("Release", juce::NotificationType::dontSendNotification);
    compMixSliderLabel.setText("Mix", juce::NotificationType::dontSendNotification);

    limiterThreshSliderLabel.setText("Thresh", juce::NotificationType::dontSendNotification);
    limiterReleaseSliderLabel.setText("Release", juce::NotificationType::dontSendNotification);

    // Buttons
    for (int i = 0; i < buttons.size(); i++)
    {
        setCommonButtonProps(*buttons[i]);
    }
    frequencyButton.setRadioGroupId(1234);
    delayEffectsButton.setRadioGroupId(1234);
    reverbButton.setRadioGroupId(1234);
    dynamicButton.setRadioGroupId(1234);

    frequencyButton.onClick = [this] { buttonClicked(&frequencyButton); };
    delayEffectsButton.onClick = [this] { buttonClicked(&delayEffectsButton); };
    reverbButton.onClick = [this] { buttonClicked(&reverbButton); };
    dynamicButton.onClick = [this] { buttonClicked(&dynamicButton); };

    chorusButton.onClick = [this] { buttonClicked(&chorusButton); };
    feedbackButton.onClick = [this] { buttonClicked(&feedbackButton); };

    // Setting load IR button
    addAndMakeVisible(loadBtn);
    loadBtn.setButtonText("Load IR");
    loadBtn.onClick = [this]()
        {
            fileChooser = std::make_unique<juce::FileChooser>("Chose File",
                audioProcessor.root, "*");

            const auto fileChooserFlags = juce::FileBrowserComponent::openMode ||
                juce::FileBrowserComponent::canSelectFiles ||
                juce::FileBrowserComponent::canSelectDirectories;

            fileChooser->launchAsync(fileChooserFlags, [this](const
                juce::FileChooser& chooser)
                {
                    juce::File result(chooser.getResult());

                    if (result.getFileExtension() == ".wav" ||
                        result.getFileExtension() == ".mp3")
                    {
                        audioProcessor.savedFile = result;
                        audioProcessor.root =
                            audioProcessor.savedFile.getParentDirectory().getFullPathName();
                        audioProcessor.variableTree.setProperty("file1",
                            audioProcessor.savedFile.getFullPathName(), nullptr);
                        audioProcessor.variableTree.setProperty("root",
                            audioProcessor.savedFile.getParentDirectory().getFullPathName(),
                            nullptr);
                        audioProcessor.myConvolution.reset();
                        audioProcessor.myConvolution.loadImpulseResponse(audioProcessor.savedFile);
                        irName.setText(result.getFileName(),
                            juce::dontSendNotification);
                    }
                });
        };

    /* Comboboxes */
    // Frequency
    addAndMakeVisible(filterTypeBox);
    filterTypeBox.addItem("low-pass", 2);
    filterTypeBox.addItem("high-pass", 1);
    typeAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.treeState, typeId, filterTypeBox);

    // Delay
    addAndMakeVisible(lowFuncTypeBox);
    lowFuncTypeBox.addItem("sin(x)", 2);
    lowFuncTypeBox.addItem("tan(sin(x))", 1);
    lowFuncTypeBox.addItem("arctan(sin(x))", 3);
    lowFuncTypeBox.addListener(this);

    /* Groups */
    for (int i = 0; i < groups.size(); i++)
    {
        addAndMakeVisible(groups[i]);
    }
    processingSettingsGroup.setText("Processing Settings");
    frequencyGroup.setText("Frequency filters");
    delayEffectsGroup.setText("Delay effects");
    reverbGroup.setText("Reverb");
    dynamicProcGroup.setText("Dynamic processing");

    // Delay
    delayGroup.setText("Delay settings");
    chorusGroup.setText("Chorus");

    // Compressor
    ioGroup.setText("IO");
    compressorGroup.setText("Compressor");
    limiterGroup.setText("Limiter");
}

VKRprojectAudioProcessorEditor::~VKRprojectAudioProcessorEditor()
{
    for (auto& slider : sliders)
    {
        slider->setLookAndFeel(nullptr);
    }
    sliders.clear();
    sliders.shrink_to_fit();
}

//==============================================================================
void VKRprojectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.setGradientFill(juce::ColourGradient::vertical(juce::Colour::fromRGB(40, 42, 53).darker(0.75f),
        getHeight(), juce::Colour::fromRGB(40, 24, 53).brighter(0.02f), getHeight() * 0.4));
    g.fillRect(getLocalBounds());
}

void VKRprojectAudioProcessorEditor::resized()
{
    // Setup margins and sizes
    auto LeftMargin = getWidth() * 0.004;
    auto UpMargin = getHeight() * 0.1;

    auto SliderWidth = getWidth() * 0.14;
    auto SliderMargin = 1.1;

    auto WidthMargin = SliderMargin * 1.15;

    auto CompressorMargin = 1.37;
    auto secondRowHeight = 1.3;

    auto ButtonSize = getWidth() * 0.035;
    auto ButtonMargin = 1.25;

    /* Frequency filters */
    //Sliders
    frequencySlider.setBounds(LeftMargin, UpMargin, SliderWidth, SliderWidth);
    orderSlider.setBounds(frequencySlider.getX() + frequencySlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);

    // Combobox
    filterTypeBox.setBounds(LeftMargin * 6, UpMargin + frequencySlider.getHeight() * 1.1, SliderWidth * 1.8, ButtonSize);

    /* Delay effects*/
    // Sliders
    DelaySlider.setBounds(orderSlider.getX() + SliderWidth * 1.2, UpMargin, SliderWidth, SliderWidth);
    BalanceSlider.setBounds(DelaySlider.getX() + DelaySlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);
    WidthSlider.setBounds(BalanceSlider.getX() + BalanceSlider.getWidth() * SliderMargin, UpMargin, SliderWidth, SliderWidth);

    //Buttons
    chorusButton.setSize(ButtonSize, ButtonSize);
    feedbackButton.setSize(ButtonSize, ButtonSize);

    feedbackButton.setBounds(DelaySlider.getX(), DelaySlider.getY() + DelaySlider.getHeight() * 1.1, SliderWidth, ButtonSize);
    chorusButton.setBounds(BalanceSlider.getX(), BalanceSlider.getY() + BalanceSlider.getHeight() * 1.1, SliderWidth, ButtonSize);

    // Combobox
    lowFuncTypeBox.setBounds(WidthSlider.getX(), WidthSlider.getY() + WidthSlider.getHeight() * 1.1, SliderWidth, ButtonSize);

    // Groups
    delayGroup.setBounds(orderSlider.getX() + SliderWidth * 1.1, UpMargin / 2.5, SliderWidth * 2.13, feedbackButton.getY() + feedbackButton.getHeight() * 0.55);
    chorusGroup.setBounds(WidthSlider.getX() * 0.98, UpMargin / 2.5, SliderWidth * 1.15, chorusButton.getY() + chorusButton.getHeight() * 0.55);

    /* Compressor */
    // Input/Output
    inputSlider.setBounds(LeftMargin, delayGroup.getY() + delayGroup.getHeight() * CompressorMargin, SliderWidth, SliderWidth);
    outputSlider.setBounds(LeftMargin, inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

    ioGroup.setBounds(LeftMargin * 2.7, inputSlider.getY() * 0.86, SliderWidth * 0.9, outputSlider.getY() / 1.5);

    // Compressor
    threshSlider.setBounds(inputSlider.getX() + inputSlider.getWidth() * SliderMargin, inputSlider.getY(), SliderWidth, SliderWidth);
    compMixSlider.setBounds(threshSlider.getX() + threshSlider.getWidth(), threshSlider.getY() + threshSlider.getHeight() * secondRowHeight / 2, SliderWidth, SliderWidth);
    ratioSlider.setBounds(compMixSlider.getX() + compMixSlider.getWidth(), threshSlider.getY(), SliderWidth, SliderWidth);
    attackSlider.setBounds(threshSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);
    releaseSlider.setBounds(ratioSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

    compressorGroup.setBounds(threshSlider.getX(), ioGroup.getY(), SliderWidth * 3, ioGroup.getHeight());

    // Limiter
    limiterThreshSlider.setBounds(ratioSlider.getX() + ratioSlider.getWidth() * 1.15, threshSlider.getY(), SliderWidth, SliderWidth);
    limiterReleaseSlider.setBounds(limiterThreshSlider.getX(), inputSlider.getY() + inputSlider.getHeight() * secondRowHeight, SliderWidth, SliderWidth);

    limiterGroup.setBounds(limiterThreshSlider.getX() * 1., ioGroup.getY(), SliderWidth * 1.1, ioGroup.getHeight());

    /* Processing settings */
    frequencyButton.setBounds(WidthSlider.getX() + WidthSlider.getWidth() * 1.25, UpMargin / 2.5, SliderWidth, ButtonSize);
    delayEffectsButton.setBounds(frequencyButton.getX(), frequencyButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);
    reverbButton.setBounds(frequencyButton.getX(), delayEffectsButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);
    dynamicButton.setBounds(frequencyButton.getX(), reverbButton.getY() + ButtonSize * ButtonMargin, SliderWidth, ButtonSize);

    /* Reverb */
    loadBtn.setBounds(frequencyButton.getX() * 1.018, (dynamicButton.getY() + ButtonSize * ButtonMargin) * 1.15, SliderWidth, ButtonSize);
    irName.setBounds(loadBtn.getX() * 0.99, loadBtn.getY() + loadBtn.getHeight() * 1.25, SliderWidth, ButtonSize);

    /* Remain gruops */
    frequencyGroup.setBounds(LeftMargin, UpMargin / 6, SliderWidth * 2, filterTypeBox.getY() + ButtonSize * 1.22);
    delayEffectsGroup.setBounds(orderSlider.getX() + SliderWidth, UpMargin / 6, SliderWidth * 3.5, feedbackButton.getY() + feedbackButton.getHeight() * 1.22);
    dynamicProcGroup.setBounds(LeftMargin, inputSlider.getY() * 0.8, SliderWidth * 5.6, ioGroup.getHeight() * 1.1);
    processingSettingsGroup.setBounds(WidthSlider.getX() + WidthSlider.getWidth() * 1.2, UpMargin / 6, SliderWidth * 1.3, ButtonSize * ButtonMargin * 4.2);
    reverbGroup.setBounds(processingSettingsGroup.getX(), dynamicButton.getY() + ButtonSize * ButtonMargin, SliderWidth * 1.3, ButtonSize * 5);
}
