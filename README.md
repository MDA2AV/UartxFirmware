# STM32 GPIO Control and UART Communication Project

This project is a demonstration of GPIO pin control and sensor state management on an STM32 microcontroller, facilitated through UART communication. It provides functionality for resetting outputs, reading sensor inputs, and dynamically setting GPIO states based on commands received via UART.

## Table of Contents

1. [Introduction](#introduction)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)
4. [Software Requirements](#software-requirements)
5. [Command Specifications](#command-specifications)
6. [Detailed Workflow](#detailed-workflow)
7. [Setup Instructions](#setup-instructions)
8. [Testing and Usage](#testing-and-usage)
9. [Troubleshooting](#troubleshooting)
10. [License](#license)

---

## Introduction

This project showcases the use of GPIOs on an STM32 microcontroller to interact with sensors and external devices, using UART as the primary communication interface. The microcontroller can:
- Reset GPIO outputs.
- Read states from connected sensors.
- Dynamically adjust GPIO states based on UART commands.

The system is designed for real-time responsiveness and can be easily extended for more complex use cases.

---

## Features

### Command-Driven GPIO Control
- **`r`**: Reset all outputs.
- **`d`**: Read the states of two sensors connected to GPIO pins and return a corresponding response over UART.
- **`sXY\n`**: Dynamically set the state of GPIO_PIN_8 or GPIO_PIN_9 based on `X` (pin selection) and `Y` (state).

### UART Echo
- Commands received via UART, except those starting with `d`, are echoed back for confirmation.

### Sensor Input Reading
- Reads digital inputs from two GPIO pins (PC0, PC1) and determines a corresponding state:
  - `00` → `'0'`
  - `01` → `'1'`
  - `10` → `'2'`
  - `11` → `'3'`

---

## Hardware Requirements

- **Microcontroller**: STM32 (any compatible device with UART and GPIO support).
- **Peripherals**:
  - 2 GPIO pins configured as inputs (PC0 and PC1 for sensors).
  - 2 GPIO pins configured as outputs (PC8 and PC9 for external devices).
  - UART connection for communication.
- **Sensors**: 2 digital sensors for GPIO input testing.
- **External Devices**: LEDs or other devices for GPIO output testing.

---

## Software Requirements

- STM32CubeMX for hardware configuration.
- STM32 HAL (Hardware Abstraction Layer) library.
- Keil µVision, STM32CubeIDE, or any other IDE compatible with STM32.
- Serial terminal emulator (e.g., PuTTY, Tera Term) for UART communication.

---

## Command Specifications

| Command       | Description                                                       | Example       |
|---------------|-------------------------------------------------------------------|---------------|
| `r`           | Resets GPIO_PIN_8 and GPIO_PIN_9 to LOW.                         | `r`           |
| `d`           | Reads sensor states and transmits a corresponding value.         | `d`           |
| `sXY\n`       | Sets GPIO_PIN_8 (`X=0`) or GPIO_PIN_9 (`X=1`) to HIGH (`Y=1`) or LOW (`Y=0`). | `s11\n`       |

---

## Detailed Workflow

1. **UART Communication**:
   - The system continuously listens for commands over UART.
   - Commands are expected to be 5 characters long (e.g., `sXY\n`).

2. **GPIO Control**:
   - GPIO_PIN_8 and GPIO_PIN_9 are controlled based on `sXY\n` commands.

3. **Sensor Input Handling**:
   - Reads states from GPIO_PIN_0 and GPIO_PIN_1.
   - Maps their states (`00`, `01`, `10`, `11`) to characters (`'0'`, `'1'`, `'2'`, `'3'`) for UART transmission.

4. **Echo Functionality**:
   - All commands except `d` are echoed back over UART.

---

## Setup Instructions

1. **Hardware Setup**:
   - Connect sensors to GPIO_PIN_0 (PC0) and GPIO_PIN_1 (PC1).
   - Connect LEDs or other output devices to GPIO_PIN_8 (PC8) and GPIO_PIN_9 (PC9).
   - Establish a UART connection between the STM32 and your PC.

2. **Software Configuration**:
   - Use STM32CubeMX to configure the GPIOs and UART peripherals:
     - GPIO_PIN_8 and GPIO_PIN_9 as outputs.
     - GPIO_PIN_0 and GPIO_PIN_1 as inputs.
     - UART (e.g., USART3) for communication.
   - Generate code and integrate the provided source code into the project.

3. **Flashing the Firmware**:
   - Compile the code and flash it onto the STM32 microcontroller using your preferred IDE.

---

## Testing and Usage

1. **Connect to a Serial Terminal**:
   - Use software like PuTTY or Tera Term to establish a UART connection.
   - Configure the terminal with the appropriate baud rate and settings.

2. **Send Commands**:
   - Test the `r`, `d`, and `sXY\n` commands to verify functionality.
   - Observe the GPIO outputs (LEDs or external devices) for expected behavior.

3. **Verify Sensor Inputs**:
   - Change the states of the sensors and send the `d` command.
   - Confirm the correct response (`0`, `1`, `2`, or `3`) is received via UART.

---

## Troubleshooting

1. **No UART Communication**:
   - Verify the UART baud rate and terminal settings.
   - Check the wiring between the STM32 and the UART interface.

2. **Incorrect GPIO Behavior**:
   - Ensure the GPIO pins are correctly configured as inputs or outputs.
   - Check for hardware-level issues (e.g., loose connections, faulty sensors).

3. **Unexpected Responses**:
   - Confirm that commands sent over UART are correctly formatted.
   - Debug using breakpoints to verify the program flow.

---

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.

---

## Author

Developed by [Your Name]. For any queries, suggestions, or improvements, feel free to contact [your.email@example.com].