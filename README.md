# MikroC PIC16F877A Projects

A collection of embedded C programs written in **MikroC PRO for PIC**, targeting the **PIC16F877A** microcontroller.

---

## 📋 Table of Contents

- [About](#about)
- [Requirements](#requirements)
- [Projects](#projects)
- [Getting Started](#getting-started)
- [PIC16F877A Overview](#pic16f877a-overview)
- [License](#license)

---

## About

This repository contains MikroC source files (`.cc`) for the PIC16F877A 8-bit microcontroller. Each file demonstrates a specific peripheral or concept such as timers, interrupts, and display interfacing.

---

## Requirements

- **IDE:** [MikroC PRO for PIC](https://www.mikroe.com/mikroc-pic) (any recent version)
- **Microcontroller:** PIC16F877A
- **Programmer:** PICkit 2 / PICkit 3 or compatible
- **Simulator (optional):** Proteus Design Suite

---

## Projects

| File | Description |
|------|-------------|
| `MikroC/A.cc` | *(work in progress)* |
| `MikroC/B.cc` | *(work in progress)* |
| `MikroC/C.cc` | **7-Segment Display Counter** – counts 0–9 on a common-cathode 7-segment display using Timer0 interrupts (~1 second interval). Output on PORTC. |

### C.cc – 7-Segment Counter Detail

- Uses **Timer0** with a 1:256 prescaler and internal clock.
- Fires an interrupt every ~10 ms; counts up to 100 ticks (≈ 1 second) before advancing the digit.
- Drives a **common-cathode** 7-segment display connected to **PORTC**.
- Automatically wraps back to `0` after reaching `9`.

---

## Getting Started

1. Clone the repository:
   ```bash
   git clone https://github.com/najzdev/MikroC-PIC16F877A.git
   ```
2. Open the desired `.cc` file in **MikroC PRO for PIC**.
3. Set the target device to **PIC16F877A** and configure the oscillator frequency to match your hardware (typically 4 MHz or 8 MHz).
4. Build the project and program the chip using your preferred programmer.

---

## PIC16F877A Overview

| Feature | Value |
|---------|-------|
| Architecture | 8-bit PIC (mid-range) |
| Program Memory | 8 KB Flash |
| RAM | 368 bytes |
| I/O Pins | 33 |
| Timers | Timer0, Timer1, Timer2 |
| ADC | 10-bit, 8 channels |
| Communication | USART, SPI, I²C |
| Operating Voltage | 2.0 – 5.5 V |

---

## License

This project is open-source and available under the [MIT License](LICENSE).
