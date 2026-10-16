#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <ctype.h>
#include "uart.h"
#include "timer.h"

#define CMD_BUFFER_SIZE 48
#define LED_PIN PB5 // Arduino Pin 13 / Port B Pin 5

static void init_hardware(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    PORTB &= ~(1 << LED_PIN); // Initially OFF

    timer_init();
    uart_init(UART_BAUD_RATE);
    sei(); // Enable global interrupts
}

static void trim_trailing(char *str) {
    int i = strlen(str) - 1;
    while (i >= 0 && (str[i] == '\r' || str[i] == '\n' || str[i] == ' ')) {
        str[i] = '\0';
        i--;
    }
}

static void process_command(char *cmd) {
    trim_trailing(cmd);
    if (strlen(cmd) == 0) return;

    if (strcasecmp(cmd, "HELP") == 0) {
        uart_write_string("\r\n--- Available Commands ---\r\n");
        uart_write_string(" HELP        - Show this manual\r\n");
        uart_write_string(" STATUS      - System uptime and state\r\n");
        uart_write_string(" LED ON      - Turn ON onboard LED\r\n");
        uart_write_string(" LED OFF     - Turn OFF onboard LED\r\n");
        uart_write_string(" LED TOGGLE  - Invert onboard LED state\r\n");
        uart_write_string(" ECHO <text> - Returns input string\r\n");
        uart_write_string("---------------------------\r\n> ");
    }
    else if (strcasecmp(cmd, "STATUS") == 0) {
        uart_write_string("\r\n[OK] System: RUNNING | Uptime: ");
        uart_write_int(timer_get_millis() / 1000);
        uart_write_string(" seconds | LED State: ");
        if (PINB & (1 << LED_PIN)) {
            uart_write_string("ON\r\n> ");
        } else {
            uart_write_string("OFF\r\n> ");
        }
    }
    else if (strcasecmp(cmd, "LED ON") == 0) {
        PORTB |= (1 << LED_PIN);
        uart_write_string("\r\n[OK] LED is now ON\r\n> ");
    }
    else if (strcasecmp(cmd, "LED OFF") == 0) {
        PORTB &= ~(1 << LED_PIN);
        uart_write_string("\r\n[OK] LED is now OFF\r\n> ");
    }
    else if (strcasecmp(cmd, "LED TOGGLE") == 0) {
        PORTB ^= (1 << LED_PIN);
        uart_write_string("\r\n[OK] LED toggled\r\n> ");
    }
    else if (strncasecmp(cmd, "ECHO ", 5) == 0) {
        uart_write_string("\r\n[ECHO] ");
        uart_write_string(cmd + 5);
        uart_write_string("\r\n> ");
    }
    else {
        uart_write_string("\r\n[ERR] Unknown command: '");
        uart_write_string(cmd);
        uart_write_string("'. Type HELP.\r\n> ");
    }
}

int main(void) {
    char cmd_buffer[CMD_BUFFER_SIZE];
    uint8_t cmd_idx = 0;

    init_hardware();

    uart_write_string("\r\n==========================================\r\n");
    uart_write_string("   AVR Micro-OS v1.0.0 (Bare Metal C)    \r\n");
    uart_write_string("   Non-blocking ISR UART + Timer1 CTC    \r\n");
    uart_write_string("   Ready. Type HELP for commands.        \r\n");
    uart_write_string("==========================================\r\n> ");

    while (1) {
        char c;
        if (uart_read_char(&c)) {
            // Echo character back to terminal
            uart_write_char(c);

            if (c == '\r' || c == '\n') {
                cmd_buffer[cmd_idx] = '\0';
                process_command(cmd_buffer);
                cmd_idx = 0;
            } else if (c == '\b' || c == 127) { // Handle backspace
                if (cmd_idx > 0) {
                    cmd_idx--;
                }
            } else {
                if (cmd_idx < (CMD_BUFFER_SIZE - 1)) {
                    cmd_buffer[cmd_idx++] = c;
                }
            }
        }
    }

    return 0;
}
