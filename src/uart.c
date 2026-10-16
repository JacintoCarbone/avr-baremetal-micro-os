#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

// Circular buffer structures
static volatile char rx_buffer[UART_RX_BUFFER_SIZE];
static volatile uint8_t rx_head = 0;
static volatile uint8_t rx_tail = 0;

static volatile char tx_buffer[UART_TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

void uart_init(uint32_t baud_rate) {
    // Calculate baud rate prescaler (Normal speed mode, F_CPU = 16MHz)
    uint16_t ubrr = (F_CPU / (16UL * baud_rate)) - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // Enable Receiver, Transmitter, and RX Complete Interrupt
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);

    // Frame format: 8 data bits, 1 stop bit, no parity (8N1)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Interrupt Service Routine: Byte received
ISR(USART_RX_vect) {
    char data = UDR0;
    uint8_t next_head = (rx_head + 1) % UART_RX_BUFFER_SIZE;
    if (next_head != rx_tail) {
        rx_buffer[rx_head] = data;
        rx_head = next_head;
    }
    // If buffer is full, newest character is dropped to avoid buffer overflow
}

// Interrupt Service Routine: Data register empty (Ready to transmit next byte)
ISR(USART_UDRE_vect) {
    if (tx_head != tx_tail) {
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % UART_TX_BUFFER_SIZE;
    } else {
        // Queue is empty, disable UDRE interrupt
        UCSR0B &= ~(1 << UDRIE0);
    }
}

bool uart_write_char(char c) {
    uint8_t next_head = (tx_head + 1) % UART_TX_BUFFER_SIZE;
    if (next_head == tx_tail) {
        return false; // Buffer full
    }

    cli();
    tx_buffer[tx_head] = c;
    tx_head = next_head;
    UCSR0B |= (1 << UDRIE0); // Enable transmit interrupt to trigger transmission
    sei();

    return true;
}

void uart_write_string(const char *str) {
    while (*str) {
        while (!uart_write_char(*str)); // Busy wait only if buffer is full
        str++;
    }
}

void uart_write_int(int32_t num) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%ld", (long)num);
    uart_write_string(buf);
}

bool uart_read_char(char *c) {
    if (rx_head == rx_tail) {
        return false; // Buffer empty
    }

    cli();
    *c = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % UART_RX_BUFFER_SIZE;
    sei();

    return true;
}

bool uart_is_rx_ready(void) {
    return (rx_head != rx_tail);
}
