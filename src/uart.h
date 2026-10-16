#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#define UART_BAUD_RATE 9600
#define UART_RX_BUFFER_SIZE 64
#define UART_TX_BUFFER_SIZE 64

void uart_init(uint32_t baud_rate);
bool uart_write_char(char c);
void uart_write_string(const char *str);
void uart_write_int(int32_t num);
bool uart_read_char(char *c);
bool uart_is_rx_ready(void);

#endif // UART_H
