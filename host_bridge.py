"""
Python Host Bridge for AVR Embedded Controller
Connects to the microcontroller over Serial (UART) to send commands and log telemetry.
"""

import time
import sys
import argparse

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    serial = None


def list_ports():
    if not serial:
        print("[!] pyserial is not installed. Install via: pip install pyserial")
        return []
    ports = serial.tools.list_ports.comports()
    print("Available COM Ports:")
    for p in ports:
        print(f" - {p.device}: {p.description}")
    return ports


def send_command(port: str, baud: int, command: str):
    if not serial:
        print("[!] pyserial is required. Install via: pip install pyserial")
        sys.exit(1)

    print(f"[*] Opening {port} at {baud} baud...")
    try:
        with serial.Serial(port, baud, timeout=2.0) as ser:
            time.sleep(2.0)  # Wait for DTR reset on Arduino-compatible boards
            ser.reset_input_buffer()

            full_cmd = (command.strip() + "\r\n").encode("utf-8")
            print(f"[>] Sending: {command.strip()}")
            ser.write(full_cmd)

            # Read response
            time.sleep(0.3)
            response = ser.read_all().decode("utf-8", errors="replace")
            print(f"[<] Response:\n{response}")

    except Exception as e:
        print(f"[!] Error communicating with port {port}: {e}")


def main():
    parser = argparse.ArgumentParser(description="Host Serial Bridge for AVR Micro-OS")
    parser.add_argument("--list-ports", action="store_true", help="List available serial ports")
    parser.add_argument("--port", type=str, default="COM3", help="Serial port name (e.g., COM3, /dev/ttyUSB0)")
    parser.add_argument("--baud", type=int, default=9600, help="Baud rate (default: 9600)")
    parser.add_argument("--cmd", type=str, default="STATUS", help="Command to send (e.g. HELP, STATUS, LED ON)")

    args = parser.parse_args()

    if args.list_ports:
        list_ports()
    else:
        send_command(args.port, args.baud, args.cmd)


if __name__ == "__main__":
    main()
