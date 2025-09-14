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

fetch(Juce.getBackendResourceAddress("data.json"))
  .then((response) => response.text())
  .then((textFromBackend) => {
    console.log(textFromBackend);
  });

document.addEventListener("DOMContentLoaded", () => {
  const button = document.getElementById("nativeFunctionButton");
  button.addEventListener("click", () => {
    nativeFunction("one", 2, null).then((result) => {
      console.log(result);
    });
  });

  const emitEventButton = document.getElementById("emitEventButton");
  let emittedCount = 0;
  emitEventButton.addEventListener("click", () => {
    window.__JUCE__.backend.emitEvent("exampleJavaScriptEvent", {
      emittedCount: emittedCount,
    });
  });

  window.__JUCE__.backend.addEventListener("outputLevel", () => {
    fetch(Juce.getBackendResourceAddress("outputLevel.json"))
      .then((response) => response.text())
      .then((outputLevel) => {
        const levelData = JSON.parse(outputLevel);
        console.log(levelData);
        Plotly.animate(
          "outputLevelPlot",
          {
            data: [
              {
                y: [levelData.left - base],
              },
            ],
            traces: [0],
            layout: {},
          },
          {
            transition: {
              duration: 20,
              easing: "cubic-in-out",
            },
            frame: {
              duration: 20,
            },
          }
        );
      });
  });
});
