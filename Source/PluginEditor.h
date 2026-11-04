#pragma once
#include "PluginProcessor.h"

class TunerAudioProcessorEditor : public juce::AudioProcessorEditor,
                                  private juce::Timer
{
public:
    explicit TunerAudioProcessorEditor (TunerAudioProcessor&);
    ~TunerAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    float detectPitch();

    TunerAudioProcessor& proc;
    juce::Slider a4Slider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> a4Attachment;

    float freq = -1.0f;
    int   missed = 0;
    juce::String noteName { "--" };
    float cents = 0.0f;
    bool  active = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TunerAudioProcessorEditor)
};
