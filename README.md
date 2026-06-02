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
├── STM32_Drivers/
│   ├── GPIO_Driver/
│   ├── SPI_Driver/
│   ├── I2C_Driver/
│   ├── USART_Driver/
│   ├── RCC_Driver/
│   └── EXTI_Driver/
│
├── Peripheral_Projects/
│   ├── LED_Control/
│   ├── Button_Interrupts/
│   ├── UART_Communication/
│   ├── SPI_Communication/
│   ├── I2C_Communication/
│   └── ADC_Applications/
│
├── FreeRTOS_Projects/
│   ├── Task_Management/
│   ├── Queue_Examples/
│   ├── Semaphore_Examples/
│   ├── Mutex_Examples/
│   └── Event_Group_Examples/
│
├── Embedded_Applications/
│   ├── Sensor_Interfacing/
│   ├── Data_Logging_System/
│   ├── LCD_Display_Project/
│   ├── RTC_Applications/
│   └── Communication_Protocols/
│
├── Documentation/
│   ├── STM32_Reference_Notes/
│   ├── Driver_Architecture/
│   ├── Peripheral_Guides/
│   └── Project_Reports/
│
├── Images/
│   ├── Circuit_Diagrams/
│   ├── Hardware_Setups/
│   └── Output_Screenshots/
│
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
