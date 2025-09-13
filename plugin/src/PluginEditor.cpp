#include "JuceWebViewPlugin/PluginEditor.h"
#include "JuceWebViewPlugin/PluginProcessor.h"

namespace webview_plugin {
namespace {
auto streamToVector(juce::InputStream& stream) {
  using namespace juce;
  std::vector<std::byte> result((size_t)stream.getTotalLength());
  stream.setPosition(0);
  [[maybe_unused]] const auto bytesRead =
      stream.read(result.data(), result.size());
  jassert(bytesRead == (ssize_t)result.size());
  return result;
}

const char* getMimeForExtension(const juce::String& extension) {
  using namespace juce;
  static const std::unordered_map<String, const char*> mimeMap = {
      {{"htm"}, "text/html"},
      {{"html"}, "text/html"},
      {{"txt"}, "text/plain"},
      {{"jpg"}, "image/jpeg"},
      {{"jpeg"}, "image/jpeg"},
      {{"svg"}, "image/svg+xml"},
      {{"ico"}, "image/vnd.microsoft.icon"},
      {{"json"}, "application/json"},
      {{"png"}, "image/png"},
      {{"css"}, "text/css"},
      {{"map"}, "application/json"},
      {{"js"}, "text/javascript"},
      {{"woff2"}, "font/woff2"}};

  if (const auto it = mimeMap.find(extension.toLowerCase());
      it != mimeMap.end())
    return it->second;

  jassertfalse;
  return "";
}
}  // namespace

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processorRef(p),
      webView{juce::WebBrowserComponent::Options{}
                  .withBackend(
                      juce::WebBrowserComponent::Options::Backend::webview2)
                  .withWinWebView2Options(
                      juce::WebBrowserComponent::Options::WinWebView2{}
                          .withUserDataFolder(juce::File::getSpecialLocation(
                              juce::File::tempDirectory))
                          .withBackgroundColour(juce::Colours::white))
                  .withResourceProvider(
                      [this](const auto& url) { return getResource(url); })
                  .withNativeIntegrationEnabled()
                  .withUserScript(R"(console.log("C++ backend");)")
                  .withInitialisationData("vendor", JUCE_COMPANY_NAME)
                  .withInitialisationData("pluginName", JUCE_PRODUCT_NAME)
                  .withInitialisationData("pluginVersion", JUCE_PRODUCT_VERSION)
                  .withNativeFunction(
                      juce::Identifier{"nativeFunction"},
                      [this](const juce::Array<juce::var>& args,
                             juce::WebBrowserComponent::NativeFunctionCompletion
                                 completion) {
                        nativeFunction(args, std::move(completion));
                      })} {
  juce::ignoreUnused(processorRef);

  addAndMakeVisible(webView);

  webView.goToURL(webView.getResourceProviderRoot());

  runJavaScriptButton.onClick = [this] {
    constexpr auto JAVASCRIPT_TO_RUN{"console.log(\"Hello from C++!\")"};
    webView.evaluateJavascript(
        JAVASCRIPT_TO_RUN,
        [](juce::WebBrowserComponent::EvaluationResult result) {
          if (const auto* resultPtr = result.getResult()) {
            std::cout << "JavaScript evaluation result:"
                      << resultPtr->toString() << std::endl;
          } else {
            std::cout << "JavaScript evaluation failed because:"
                      << result.getError()->message << std::endl;
          }
        });
  };

  addAndMakeVisible(runJavaScriptButton);

  emitJavaScriptButton.onClick = [this] {
    static const juce::Identifier EVENT_ID{"exampleEvent"};
    webView.emitEventIfBrowserIsVisible(EVENT_ID, 42.0);
  };

  addAndMakeVisible(emitJavaScriptButton);

  addAndMakeVisible(labelUpdatedFromJavaScript);

  setResizable(true, true);
  setSize(800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::resized() {
  auto bounds = getLocalBounds();
  webView.setBounds(bounds.removeFromRight(getWidth() / 2));

  runJavaScriptButton.setBounds(bounds.removeFromTop(50).reduced(5));
  emitJavaScriptButton.setBounds(bounds.removeFromTop(50).reduced(5));
  labelUpdatedFromJavaScript.setBounds(bounds.removeFromTop(50).reduced(5));
}

auto AudioPluginAudioProcessorEditor::getResource(const juce::String& url)
    -> std::optional<Resource> {
  std::cout << url << std::endl;

  static const auto resourceFileRoot = juce::File{
      R"(C:\Users\tatsu\dev\repos\juce-webview-tutorial\plugin\ui\public)"};

  const auto resourceToRetrieve =
      url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);
  const auto resource =
      resourceFileRoot.getChildFile(resourceToRetrieve).createInputStream();

  if (resource) {
    const auto extension =
        resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
    return Resource{streamToVector(*resource), getMimeForExtension(extension)};
  }

  return std::nullopt;
}

void AudioPluginAudioProcessorEditor::nativeFunction(
    const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) {
  juce::String concatenatedArgs;

  for (const auto& arg : args) {
    concatenatedArgs += arg.toString();
  }

  labelUpdatedFromJavaScript.setText("Native function:" + concatenatedArgs,
                                     juce::dontSendNotification);

  completion("nativeFunction callback: All OK!");
}

}  // namespace webview_plugin
