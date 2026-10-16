# Bare-Metal AVR Micro-OS & Embedded Command Engine

A lightweight, high-efficiency embedded operating core written in **pure C99** for the Microchip/Atmel **ATmega328P** (Arduino Uno/Nano architecture).

Built **without Arduino framework abstractions** for direct hardware control, minimal RAM overhead, and predictable real-time execution.

---

## Technical Highlights

- **Direct Register Manipulation**: Hardware access via `DDRB`, `PORTB`, `UBRR0`, `UCSR0B`, and `TIMSK1` for zero abstraction latency.
- **Asynchronous Ring-Buffer UART**: Full-duplex, non-blocking serial communication using 64-byte circular buffers driven by `USART_RX_vect` and `USART_UDRE_vect` interrupts.
- **Hardware Timer1 CTC Scheduler**: 16-bit Timer/Counter1 configured in Clear Timer on Compare Match (CTC) mode generating an exact 1 ms system tick (`TIMER1_COMPA_vect`).
- **Interactive Command Interpreter (CLI)**: Onboard ASCII protocol parser handling `HELP`, `STATUS`, `LED ON/OFF/TOGGLE`, and `ECHO`.
- **Memory Footprint**: ~1.3 KB Flash and < 190 bytes SRAM (leaving > 90% of MCU resources free for application logic).

---

## Repository Structure

```text
02_embedded_c_avr/
├── src/
│   ├── main.c        # Main state machine, CLI dispatcher, and GPIO initialization
│   ├── uart.h / .c   # Interrupt-driven circular buffer UART driver
│   └── timer.h / .c  # Hardware Timer1 1ms tick engine
├── host_bridge.py    # Python desktop client to communicate with the firmware
├── Makefile          # avr-gcc toolchain build automation
├── diagram.json      # Wokwi online circuit simulation schematic
└── wokwi.toml        # Wokwi project descriptor
```

---

## Run Online (Wokwi Simulator)

1. Open [Wokwi Arduino Simulator](https://wokwi.com/).
2. Import this project folder or use the VS Code Wokwi extension.
3. Start the simulation and open the Serial Monitor at **9600 baud**.
4. Type `HELP` and press Enter to interact with the firmware.

---

## Local Compilation (avr-gcc)

```bash
# Compile ELF, HEX, and display memory statistics
make

# Flash to an Arduino Uno connected via USB
make flash
```

---

## Python Host Bridge

Send commands and poll status directly from Python:
```bash
python host_bridge.py --port COM3 --cmd "STATUS"
python host_bridge.py --port COM3 --cmd "LED ON"
```
