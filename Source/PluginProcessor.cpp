#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout TunerAudioProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    l.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "a4", 1 }, "A4",
        juce::NormalisableRange<float> (415.0f, 466.0f, 0.1f), 440.0f));
    return l;
}

TunerAudioProcessor::TunerAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    for (auto& s : ring) s.store (0.0f);
}

void TunerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int n  = buffer.getNumSamples();
    const int ch = buffer.getNumChannels();
    int wp = writePos.load (std::memory_order_relaxed);

    for (int i = 0; i < n; ++i)
    {
        float s = 0.0f;
        for (int c = 0; c < ch; ++c) s += buffer.getReadPointer (c)[i];
        ring[(size_t) ((wp + i) & ringMask)].store (s / (float) juce::jmax (1, ch),
                                                    std::memory_order_relaxed);
    }
    writePos.store ((wp + n) & ringMask, std::memory_order_release);
    // звук проходит без изменений
}

juce::AudioProcessorEditor* TunerAudioProcessor::createEditor()
{
    return new TunerAudioProcessorEditor (*this);
}

void TunerAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void TunerAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TunerAudioProcessor();
}
