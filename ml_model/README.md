# Machine Learning Model

This folder contains materials related to the machine learning workflow used in the project.

## ML Task

The project uses keyword spotting to detect spoken commands.

## Target Classes

- forward
- backward
- silence / background noise

## Workflow

- collect audio samples
- label the data
- train a lightweight model in Edge Impulse
- quantize the model
- deploy the model to the Arduino Nano 33 BLE Sense

## Outcome

The deployed model supports real-time on-device inference without requiring Wi-Fi or connection to a separate computer.
