#include <Arduino.h>
#include "Wire.h"
#include "LiquidCrystal_PCF8574.h"
#include "project.h"
#include "display.h"

static LiquidCrystal_PCF8574 lcd(0x27);
static unsigned long time_menu = 0;

static void (*button_callaback_vector[8])(uint8_t port) = {0};
static uint8_t button_callaback_mask = 0;
static uint8_t button_pressed_mask = 0;

/**
 *  | 00 01 02 03 04 05 06 07 08  09 10 11 12 13 14 15
 *  |  L  I  M  I  T  E  :                  9  9  9  9
 */

/**
 *  | 00 01 02 03 04 05 06 07 08  09 10 11 12 13 14 15
 *  |  C  :  9  9  9  9     |         L  :  9  9  9  9
 *  |  A  :  9  9  9  9
 */

void display_init()
{
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.setBacklight(1);
  lcd.clear();
}

void display_clear()
{
  lcd.clear();
}

bool display_frame(display_frame_t *frame)
{
  static char str_buffer[17];

  if (!(frame))
    return false;

  if (!(frame->data))
    return false;

  if (!(frame->format))
    return false;

  if (!(frame->type < DISPLAY_TYPE_MAX))
    return false;

  /* Prepare data format */

  switch (frame->type)
  {
  case DISPLAY_TYPE_U8:
    snprintf(str_buffer, 17, frame->format, *((uint8_t *)frame->data));
    break;
  case DISPLAY_TYPE_I8:
    snprintf(str_buffer, 17, frame->format, *((int8_t *)frame->data));
    break;
  case DISPLAY_TYPE_U16:
    snprintf(str_buffer, 17, frame->format, *((uint16_t *)frame->data));
    break;
  case DISPLAY_TYPE_I16:
    snprintf(str_buffer, 17, frame->format, *((int16_t *)frame->data));
    break;
  case DISPLAY_TYPE_U32:
    snprintf(str_buffer, 17, frame->format, *((uint32_t *)frame->data));
    break;
  case DISPLAY_TYPE_I32:
    snprintf(str_buffer, 17, frame->format, *((int32_t *)frame->data));
    break;
  case DISPLAY_TYPE_STR:
    snprintf(str_buffer, 17, frame->format, (char *)frame->data);
    break;
  default:
    return false;
    break;
  }

  /* Send output to lcd */

  lcd.setCursor(frame->pos_x, frame->pos_y);
  lcd.print(str_buffer);

  return true;
}

void menu_button_init()
{
  PCICR |= bit(PCIE0); // Enable PORT B ISR
}

void menu_button_loop()
{
  if (button_pressed_mask == 0)
    return;

  for (uint8_t pin = 0; pin < 8; pin++)
  {
    if ((button_pressed_mask & bit(pin)) == 0)
      continue;

    button_callaback_vector[pin](button_pressed_mask);
  }

  button_pressed_mask = 0;
}

void menu_button_isr(const unsigned long *ts)
{
  if (*ts - time_menu < BUTTON_DEBOUNCE_MS)
    return;

  button_pressed_mask |= (~(PINB) & button_callaback_mask); /* LOW VALUE = PRESSED*/
  time_menu = *ts;
}

bool menu_button_config(uint8_t button, void (*f)(uint8_t port))
{  
  if (!(button < 8))
    return false;

  if (f == NULL)
    return false;

  DDRB &= ~(bit(button)); /* input */
  PORTB |= bit(button);   /* pull-up */
  PCMSK0 |= bit(button);  /* enable pin change interrupt */

  button_callaback_vector[button] = f;
  button_callaback_mask |= bit(button);
  return true;
}

void menu_buttons_activate()
{
  PCICR |= bit(PCIE0); // Enable PORT B ISR
}

void menu_buttons_deactivate()
{
  PCICR &= ~bit(PCIE0); // Enable PORT B ISR
}