Project Overview: Closed-Loop Drive System

### STM32_ClosedLoop_Drive video demonstration 

[![Watch video](https://img.youtube.com/vi/i_icI5O8THY/maxresdefault.jpg)](https://www.youtube.com/watch?v=i_icI5O8THY)

*Click on the image above to go to watch the video on YouTube*

### STM32_ClosedLoop_Drive printed circuit board
<img width="940" height="727" alt="Знімок екрана 2026-05-22 155104" src="https://github.com/user-attachments/assets/fcc2e99e-c863-4274-9e0c-054161d413b2" />

<img width="1028" height="610" alt="Знімок екрана 2026-05-22 155144" src="https://github.com/user-attachments/assets/43a33a32-3432-4a42-9bb3-cde67a3d1eea" />

<img width="835" height="803" alt="Знімок екрана 2026-05-22 155350" src="https://github.com/user-attachments/assets/df593df4-488a-4a20-8f8a-d4754a2b5fc8" />

This project is a multi-node closed-loop drive control system consisting of two main processing units: STM32F412RET and STM32F103 (Blue Pill).
1. Motion Control Node (STM32F412RET)

The core of the system, responsible for high-speed motor regulation and data management:

    Motor Control: Implements PID regulation via PWM, utilizing encoder feedback and sensor measurements for precise motion.

    Data Logging: Performs real-time logging of system parameters to an SD card (FATFS, .csv files) and via UART.

    Operating System: Runs on FreeRTOS to ensure deterministic multitasking.

    Diagnostics: Supports SEGGER debugging. All communication protocols are optimized using DMA and Interrupt (IT) functions within the OSAL layer.

2. User Interface Node (STM32F103)

A dedicated MCU for human-machine interaction:

    I/O Peripherals: Features an OLED display, physical buttons, and a potentiometer for manual speed control.

    Control: Acts as the primary input for setpoints and system monitoring.

3. Communication & Architecture

    Inter-node Communication: The MCUs communicate via a CAN bus using a custom command table where each command is assigned a specific priority.

    Software Design: The project follows a strict 3-level architecture:

        BSP (Board Support Package): Hardware abstraction and HAL-level drivers.

        OSAL (Operating System Abstraction Layer): Integration of FreeRTOS features, thread-safe DMA operations, and synchronization.

        APP (Application Layer): High-level logic for user applications and control algorithms.
