# Bluetooth RC Car with Dual Ultrasonic Safety System

A smart Arduino-powered Remote Controlled (RC) car featuring Bluetooth communication (HC-05), directional LED indicators, active audio alerts, and dual ultrasonic sensors for active obstacle collision avoidance. Use any type of Serial Connection app on to control. Bugs have been found for trying to connect to PC. Use mobile for stability.

## Required Components & Bill of Materials (BOM)

To build this RC car, you will need the following hardware components, power supplies, tools, and software:

### 1. Core Electronics & Microcontroller

* **1x Arduino Uno R3** (or Arduino Nano / Mega)

* **1x HC-05 Bluetooth Module** (4-pin or 6-pin breakout board)

* **1x L298N Dual H-Bridge Motor Driver Board**

* **4x DC Gear Motors with Wheels** (TT Motors, 3V–6V ratio 1:48)

### 2. Sensors & Indicators

* **2x HC-SR04 Ultrasonic Distance Sensors** (Front & Rear obstacle detection)

* **4x 5mm LEDs:**

  * 1x Red LED (Reverse/Brake light)

  * 1x White/Bright LED (Headlight)

  * 2x Yellow/Orange LEDs (Left & Right Turn Indicators)

* **4x** $220\Omega$ **Current-Limiting Resistors** (for LEDs)

* **1x 5V Active Buzzer** (Audio alarm)

### 3. Power Supply & Hardware Accessories

* **2x 18650 3.7V Rechargeable Li-ion Batteries** (yielding $7.4\text{V}$ total power)

* **1x 18650 Battery Holder** (Dual slot, with leads)

* **1x SPST Toggle Switch** (Main power switch)

* **1x 2WD / 4WD Robot Car Chassis Kit** (Acrylic chassis, motor brackets, screws, and wheels)

* **40x Dupont Jumper Wires** (Male-to-Male, Male-to-Female, Female-to-Female)

* **1x Mini Breadboard** or Arduino Sensor Shield V5.0 (for easy pin distribution)

### 4. Software & Apps

* **Arduino IDE** (Desktop application for uploading code)

* **Smartphone (Android/iOS)** with a Bluetooth Serial Terminal app:

  * *Serial Bluetooth Terminal* (Android)

  * *Arduino RC Controller* or equivalent custom joystick app

## System Features

* **Bluetooth Control:** Full remote steering control (`F`, `B`, `L`, `R`, `S`) via smartphone.

* **Dual Obstacle Avoidance System:** Front and Rear ultrasonic sensors continuously measure clearance ($35\text{ cm}$ threshold).

* **Automatic Emergency Braking (AEB):** Forces an immediate stop (`S`) if moving towards an obstacle.

* **Non-Blocking Safety Response:** $2.5\text{-second}$ hazard light flash and buzzer alarm upon obstacle detection without freezing motor loops.

* **Visual Driving Indicators:** Integrated headlights, reverse lights (acting as brake lights), and left/right turn indicators.

## Hardware Pinout Guide

| Component | Function / Channel | Arduino Pin | Notes | 
 | ----- | ----- | ----- | ----- | 
| **Bluetooth (HC-05)** | Software Serial RX | `Pin 2` | Connects to HC-05 TXD | 
|  | Software Serial TX | `Pin 3` | Connects to HC-05 RXD (via resistor divider recommended) | 
| **Motor Driver (L298N)** | Front Right Motor (+) | `Pin 6` | PWM speed control (`FR_motor_p`) | 
|  | Front Right Motor (-) | `Pin 5` | PWM speed control (`FR_motor_n`) | 
|  | Front Left Motor (+) | `Pin 9` | PWM speed control (`FL_motor_p`) | 
|  | Front Left Motor (-) | `Pin 10` | PWM speed control (`FL_motor_n`) | 
| **Ultrasonic Front** | Trigger | `Pin A0` | Digital Output (`FRONT_TRIG_PIN`) | 
|  | Echo | `Pin A1` | Digital Input (`FRONT_ECHO_PIN`) | 
| **Ultrasonic Back** | Trigger | `Pin A2` | Digital Output (`BACK_TRIG_PIN`) | 
|  | Echo | `Pin A3` | Digital Input (`BACK_ECHO_PIN`) | 
| **Indicators & Audio** | Right Turn LED | `Pin 12` | Active HIGH (`R_led`) | 
|  | Left Turn LED | `Pin 4` | Active HIGH (`L_led`) | 
|  | Reverse / Brake LED | `Pin 7` | Active HIGH (`Reverse_led`) | 
|  | Headlight / Flash LED | `Pin 8` | Active HIGH (`Flash_led`) | 
|  | Active Buzzer | `Pin 13` | Active HIGH (`BUZZER_PIN`) | 

## Command Protocol & Behavior Matrix

The car listens for single-byte character commands transmitted over Software Serial at **9600 baud**.

| Command | Action | Motor Outputs | Active Lighting | Audio / Safety | 
 | ----- | ----- | ----- | ----- | ----- | 
| **`F`** | **Forward** | Left & Right Forward (Speed: 255) | Headlight `ON` | Front obstacle check active | 
| **`B`** | **Backward** | Left & Right Reverse (Speed: 255) | Reverse Light `ON` | Rear obstacle check active | 
| **`L`** | **Turn Left** | Right Fwd (250) / Left Rev (250) | Left LED `ON`, Headlight `ON` | Tank pivot turn | 
| **`R`** | **Turn Right** | Right Rev (250) / Left Fwd (250) | Right LED `ON`, Headlight `ON` | Tank pivot turn | 
| **`S`** | **Stop** | Motors Disabled (Speed: 0) | Reverse Light `ON` (Brake) | Standby mode | 

## Active Safety System (Obstacle Detection)

1. **Threshold Distance:** Set to $35\text{ cm}$ (`OBSTACLE_DISTANCE_CM = 35`).

2. **Trigger Condition:**

   * Command is **`F`** AND Front Sensor Distance $\le 35\text{ cm}$

   * Command is **`B`** AND Rear Sensor Distance $\le 35\text{ cm}$

3. **Safety Intervention:**

   * System overrides movement state to **`S` (Stop)** immediately.

   * Activates **Buzzer** for **2.5 seconds** ($2500\text{ ms}$).

   * Flashes **Left and Right indicator LEDs** simultaneously as hazard lights for **2.5 seconds**.

   * Output log to Serial Monitor: `"Obstacle Detected! Forcing Stop."`

## Pre-Start Checklist & Operating Steps

### 1. Hardware Connections Verification

* \[ \] Verify HC-05 TX is connected to Arduino **Pin 2** and RX is connected to Arduino **Pin 3**.

* \[ \] Ensure motor driver ground is linked to Arduino **GND**.

* \[ \] Confirm Ultrasonic sensor VCC is connected to stable 5V, not 3.3V.

### 2. Uploading Code & Serial Monitor Setup

* \[ \] Disconnect HC-05 `VCC` or `RX/TX` temporarily while uploading to avoid serial conflicts.

* \[ \] Set Arduino IDE Serial Monitor to **9600 Baud Rate**.

* \[ \] Power on the chassis and confirm the Serial Monitor displays `"HC-05 with Arduino - Ready"`.

### 3. Bluetooth Pairing & Control

* \[ \] Open your Smartphone Bluetooth Controller App (e.g., *Serial Bluetooth Terminal* or *Arduino RC Controller*).

* \[ \] Pair with **HC-05** (Default PIN: `1234` or `0000`).

* \[ \] Configure buttons or continuous tilt control to send upper-case characters: `'F'`, `'B'`, `'L'`, `'R'`, `'S'`.

## Troubleshooting Guide

* **Car doesn't turn when pressing `L` or `R`:**

  * Tank turns require higher starting torque. If batteries drop below $\sim 7\text{V}$, wheels may lock up. Recharge or replace motor power cells.

* **Obstacle detection triggers randomly:**

  * Ensure Ultrasonic sensors are mounted completely flat and perpendicular to the floor to avoid detecting ground reflection.

* **Bluetooth doesn't receive commands:**

  * Confirm that your phone app sends raw ASCII characters without extra carriage return (`\r`) or newline (`\n`) flags attached.

Thank You!
A project by Emin14614!
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
