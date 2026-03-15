# Voice-Controlled Robotic Subsystem

This project presents the development of a voice-controlled robotic subsystem that responds to spoken commands using on-device machine learning.

The subsystem was designed to recognize simple voice commands such as "forward" and "backward" and then control a stepper motor to rotate in the corresponding direction. The project combines embedded systems, machine learning, and electromechanical control in a compact real-time setup.

## Project Overview

The system uses an Arduino Nano 33 BLE Sense, which includes a built-in microphone and onboard processing capability, together with a 28BYJ-48 stepper motor and ULN2003 motor driver. A keyword spotting model was trained using Edge Impulse and deployed directly to the microcontroller for offline real-time inference.

## Objectives

- Build a robotic subsystem that reacts to voice commands
- Train a lightweight machine learning model for keyword spotting
- Deploy the model to a microcontroller for on-device inference
- Control motor movement based on recognized commands
- Demonstrate the integration of TinyML with embedded robotics

## Key Features

- Voice-based command recognition
- TinyML keyword spotting with Edge Impulse
- Arduino Nano 33 BLE Sense deployment
- Offline real-time inference
- Stepper motor control using ULN2003 driver
- Forward and backward motor rotation
- No Wi-Fi or external computer required during operation

## Hardware Components

- Arduino Nano 33 BLE Sense
- Built-in microphone
- 28BYJ-48 stepper motor
- ULN2003 stepper motor driver board
- Jumper wires
- USB power/programming interface

## Software and Tools

- Arduino IDE
- Edge Impulse
- Embedded C/C++
- Stepper library
- TinyML deployment workflow

## How the System Works

1. Audio samples of spoken words are collected.
2. The words "forward" and "backward" are used as target classes, with silence or background noise added for balance.
3. A keyword spotting model is trained in Edge Impulse.
4. The trained model is quantized and deployed to the Arduino Nano 33 BLE Sense.
5. The microcontroller continuously listens through the built-in microphone.
6. When the command "forward" is detected, the stepper motor rotates in one direction.
7. When the command "backward" is detected, the stepper motor rotates in the opposite direction.

## Repository Structure

- `firmware/` – Arduino code for motor control and model integration
- `ml_model/` – machine learning model notes and Edge Impulse workflow
- `hardware/` – hardware setup and component information
- `system_diagrams/` – block diagrams and system flow
- `images/` – photos of the setup and screenshots
- `documentation/` – report, notes, and supporting materials

## Applications

- Voice-controlled robotics
- Human-machine interaction
- Intelligent embedded systems
- TinyML demonstration projects
- Low-power on-device AI systems

## Author

John Afriyie Oduro
