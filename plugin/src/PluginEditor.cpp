#include "JuceWebViewPlugin/PluginProcessor.h"
#include "JuceWebViewPlugin/PluginEditor.h"

namespace webview_plugin{
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    webView{juce::WebBrowserComponent::Options{}}
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible(webView);

    webView.goToURL("https://juce.com");

    setResizable(true, true);
    setSize (800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::resized()
{
    webView.setBounds(getLocalBounds());
}
}
