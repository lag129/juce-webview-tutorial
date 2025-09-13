import * as Juce from "./juce/index.js";

console.log("Hello, JS!");

window.__JUCE__.backend.addEventListener(
  "exampleEvent",
  (objectFromCppBackend) => {
    console.log(objectFromCppBackend);
  }
);

const data = window.__JUCE__.initialisationData;

document.getElementById("vendor").innerHTML = data.vendor;
document.getElementById("pluginName").innerHTML = data.pluginName;
document.getElementById("pluginVersion").innerHTML = data.pluginVersion;

const nativeFunction = Juce.getNativeFunction("nativeFunction");

document.addEventListener("DOMContentLoaded", () => {
  const button = document.getElementById("nativeFunctionButton");
  button.addEventListener("click", () => {
    nativeFunction("one", 2, null).then((result) => {
      console.log(result);
    });
  });

  let emittedCount = 0;

  const emitEventButton = document.getElementById("emitEventButton");
  emitEventButton.addEventListener("click", () => {
    window.__JUCE__.backend.emitEvent("exampleJavaScriptEvent", {
      emittedCount: emittedCount,
    });
  });
});
