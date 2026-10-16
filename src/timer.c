#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint32_t system_millis = 0;

void timer_init(void) {
    // Configure Timer1 for CTC (Clear Timer on Compare Match) mode
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, Prescaler = 64

    // 16MHz / (64 * 1000Hz) - 1 = 249 ticks per millisecond
    OCR1A = 249;

    // Enable Timer1 Compare Match A Interrupt
    TIMSK1 = (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect) {
    system_millis++;
}

uint32_t timer_get_millis(void) {
    uint32_t ms;
    cli();
    ms = system_millis;
    sei();
    return ms;
}
