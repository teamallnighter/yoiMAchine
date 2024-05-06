#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParamIDs.h"

static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    auto msFormat = [](float value, int)
    {
        if (value < 100.0f)
            return juce::String (value, 1) + " ms";
        else
            return juce::String (std::roundf (value)) + " ms";
    };

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::downsample,
                                                             "Down Sample",
                                                             juce::NormalisableRange<float> (0.0f, 30.0f, 1.0f, 0.405f),
                                                             17.0f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::cutoff, 
                                                             "CutOff Frequency",
                                                             juce::NormalisableRange<float> (0.0f, 5000.0f, 1.0f, 0.405f),
                                                             0.0f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::resonant, 
                                                             "Resonance",
                                                             juce::NormalisableRange<float> (0.0f, 1.0f, 0.1f, 0.405f),
                                                             0.5f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::attack, 
                                                             "Attack",
                                                             juce::NormalisableRange<float> (0.0f, 10000.0f, 0.01f, 0.405f),
                                                             0.0f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::decay, 
                                                             "Decay",
                                                             juce::NormalisableRange<float> (0.0f, 10000.0f, 1.0f, 0.405f),
                                                             500.0f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::sustain, 
                                                             "Sustain",
                                                             juce::NormalisableRange<float> (0.0f, 1.0f, 0.1f, 0.405f),
                                                             0.5f,
                                                             juce::String(),
                                                             juce::AudioProcessorParameter::genericParameter,
                                                             msFormat,
                                                             nullptr));

    layout.add (std::make_unique<juce::AudioParameterFloat> (ParamIDs::release, 
                                                             "Release",
                                                             juce::NormalisableRange<float> (0.0f, 10000.0f, 0.1f, 0.405f),
                                                             0.
    ));}