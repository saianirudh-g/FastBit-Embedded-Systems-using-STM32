# FastBit Embedded Systems using STM32

A comprehensive collection of embedded systems projects, drivers, and applications developed while learning STM32 microcontroller programming through the FastBit Embedded Brain Academy courses. This repository covers bare-metal programming, peripheral driver development, RTOS concepts, communication protocols, and real-world embedded applications using STM32 microcontrollers.

---

## Overview

This repository contains hands-on projects and driver development exercises using **STM32 microcontrollers**. The primary focus is understanding low-level embedded systems programming, peripheral interfacing, and RTOS-based application development.

The projects were developed using a combination of:

- STM32 Microcontrollers
- Bare-Metal Programming
- STM32CubeIDE
- FreeRTOS
- Embedded C
- Communication Protocols

---

## Learning Objectives

- Understand STM32 architecture and peripherals
- Develop reusable peripheral drivers from scratch
- Learn interrupt-driven programming
- Implement communication protocols
- Explore RTOS concepts using FreeRTOS
- Build real-world embedded applications

---

## Topics Covered

### STM32 Fundamentals

- ARM Cortex-M Architecture
- STM32 Clock Configuration
- Memory Organization
- Startup Code
- Interrupt Handling
- NVIC Configuration

### Peripheral Drivers

- GPIO Driver Development
- UART/USART Driver
- SPI Driver
- I2C Driver
- ADC Driver
- Timer Programming
- External Interrupts (EXTI)

### Communication Protocols

- UART Communication
- SPI Communication
- I2C Communication
- Serial Debugging
- Device Interfacing

### Real-Time Operating Systems

- FreeRTOS Basics
- Task Scheduling
- Queues
- Semaphores
- Mutexes
- Event Groups
- Inter-Task Communication

---

## Hardware Used

- STM32 Nucleo Development Board
- STM32F4 Discovery Board
- Push Buttons
- LEDs
- LCD Display Modules
- Sensors
- Breadboard and Jumper Wires

---

## Software Tools

| Tool | Purpose |
|--------|---------|
| STM32CubeIDE | Development Environment |
| Keil uVision | Embedded Development |
| STM32CubeMX | Peripheral Configuration |
| FreeRTOS | Real-Time Operating System |
| Git & GitHub | Version Control |

---

## Key Projects

### GPIO Driver Development

- Register-level programming
- Input and output configuration
- Interrupt support
- GPIO abstraction layer

### UART Communication

- Serial communication implementation
- Polling and interrupt modes
- Data transmission and reception

### SPI Driver

- Master-slave communication
- Full-duplex data transfer
- Sensor and display interfacing

### I2C Driver

- Multi-device communication
- EEPROM and sensor interfacing
- Address management

### FreeRTOS Applications

- Task scheduling
- Inter-task communication
- Synchronization mechanisms
- Resource sharing

---

## Skills Developed

### Embedded Programming

- Embedded C
- Register-Level Programming
- Firmware Development
- Driver Development

### Hardware Interfacing

- GPIO Control
- Sensor Interfacing
- Communication Protocols
- Embedded Debugging

### RTOS Development

- Task Management
- Scheduling Algorithms
- Synchronization Techniques
- Resource Management

---

## How to Run

### Clone Repository

```bash
git clone https://github.com/your-username/FastBit-Embedded-Systems-using-STM32.git
```

### Open Project

1. Launch STM32CubeIDE
2. Import Existing Project
3. Build the Project
4. Flash to STM32 Board
5. Monitor Output via UART or Debugger

---

## Learning Outcomes

After completing these projects, you will gain experience in:

- Embedded Systems Design
- STM32 Firmware Development
- Peripheral Driver Implementation
- RTOS-Based Application Design
- Hardware-Software Integration
- Embedded Debugging Techniques

---

## Repository Structure

```bash
FastBit-Embedded-Systems-using-STM32/
│
├── LED Blinker/Core/
│   ├── Inc/
|   ├── Src/
│   └── Startup/
│
├── LED Pins Setup/
│   ├── LED Blinking through GPIO Pins
│   ├── STM32 Nanoboard Connected
│   ├── STM32Cube Pinout Configuration
│   └── STM32F303CC
|
├── MPU6050_PWM_LED_Control/
│   ├── main.c
│   ├── app_callback.c
│   ├── kalman_filter.c
│   ├── kalman_filter.h
│   ├── mpu6050.c
│   └── mpu6050.h
|
├── STM32Cube Monitor/
│   ├── LED_Toggling with Monitor Acquistion
│   ├── Monitor Acquistions Graph 1
│   ├── Monitor Acquistions Graph 2
│   └── Monitor Hardware Connected
│
├── STM32Cube Programmer/
│   └── STM32 Connected
│
├── LED_Blinker.ioc
├── STM32F407VGTX_FLASH.ld
├── STM32F407VGTX_RAM.ld
└── README.md
```

---

## Author

**G Sai Anirudh**

Embedded Systems | VLSI | RF Engineering | IoT | Firmware Development

---

## Acknowledgements

Special thanks to **FastBit Embedded Brain Academy** for providing the course material, practical exercises, and hands-on embedded systems training.

---

## License

This repository is intended for educational, research, and learning purposes.
