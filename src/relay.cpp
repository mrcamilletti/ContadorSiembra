#include "project.h"
#include "relay.h"

static volatile unsigned long time_relay = 0;

/**
 * RELAY IO FUNCTIONS
 */

void relay_init()
{
  DDRB |= (bit(DD2));     // Set pins as outputs
  PORTB &= ~(bit(PORT2)); // Set outputs as LOW
}

uint8_t relay_status()
{
  return ((PINB & bit(PIN2)) > 0);
}

void relay_on()
{
  PORTB |= (bit(PORT2)); // Enable PORT B ISR
  time_relay = millis();
}

void relay_on_from_isr(unsigned long * ts)
{
  PORTB |= (bit(PORT2)); // Enable PORT B ISR
  time_relay = *ts;
}

void relay_off()
{
  PORTB &= ~(bit(PORT2)); // Enable PORT B ISR
}

void relay_loop()
{
  if (((PINB & bit(PIN2)) == 0))
    return;

  unsigned long ts = millis();

  if (ts - time_relay < RELAY_WINDOW_MS)
    return;

  time_relay = ts;
  relay_off();
}