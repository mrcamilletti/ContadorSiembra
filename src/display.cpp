#include <Arduino.h>
#include "Wire.h"
#include "LiquidCrystal_PCF8574.h"
#include "project.h"
#include "display.h"

static LiquidCrystal_PCF8574 lcd(0x27);
static unsigned long time_menu = 0;

static void (*button_callaback_vector[8])(uint8_t port) = { 0 };
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
  DDRC &= ~(bit(DD4) | bit(DD5));     // Set pins as inputs
  PORTC |= (bit(PORT4) | bit(PORT5)); // Set pull-up

  lcd.begin(16, 2);  
  lcd.noCursor();
  lcd.setBacklight(1);  
  lcd.clear();
}

void display_clear()
{
    lcd.clear();
}

void display_main_screen_print(display_params_t * params)
{
  if (params == NULL)
    return;

  char line0[17];
  char line1[17];  

  snprintf(line0, 17, "ACCION  | C:%4u", params->counter);
  snprintf(line1, 17, "%8lu| D:%4u", params->actions, params->limit);

  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}

void display_main_screen_update(display_params_t * params)
{
  char counter_str[5];
  char limit_str[5];
  char actions_str[10];
  
  snprintf(counter_str, 5, "%4u", params->counter);
  snprintf(limit_str, 5, "%4u", params->limit);
  snprintf(actions_str, 10, "%8lu", params->actions);

  lcd.setCursor(12,0);
  lcd.print(counter_str);  
  
  lcd.setCursor(0,1);
  lcd.print(actions_str);

  lcd.setCursor(12,1);
  lcd.print(limit_str);    
}


void display_second_screen_print(display_params_t * params)
{
  char line0[17];
  char line1[17];  

  snprintf(line0, 17, "CONFIGURACION   ");
  snprintf(line1, 17, "> Divisor:  %4u", params->limit);

  lcd.setCursor(0,0);
  lcd.print(line0);
  lcd.setCursor(0,1);
  lcd.print(line1);
}

void display_second_screen_update(display_params_t * params)
{
  char limit_str[5];
  
  snprintf(limit_str, 5, "%4u", params->limit);

  lcd.setCursor(12,1);
  lcd.print(limit_str);
}

void display_line(const char * str, uint8_t row)
{
    lcd.setCursor(0, row);
    lcd.print(str);
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

void menu_button_isr_ts(unsigned long * ts)
{
  if (*ts - time_menu < BUTTON_DEBOUNCE_MS)
    return;
  
  button_pressed_mask |= (~(PINB) & button_callaback_mask);  /* LOW VALUE = PRESSED*/  
  time_menu = *ts;
}

void menu_button_isr() {
  unsigned long ts = millis();
  menu_button_isr_ts(&ts);
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

bool menu_button_up()
{
  return ((PINB & bit(PINB0)) == 0);
}

bool menu_button_down()
{
  return ((PINB & bit(PINB1)) == 0);
}

bool menu_button_up_isr(unsigned long * ts)
{
    if (*ts - time_menu < BUTTON_DEBOUNCE_MS)
        return false;

    if ((PINB & bit(PINB0)) > 0)
      return false;
    
    time_menu = *ts;
    return true;
}

bool menu_button_down_isr(unsigned long * ts)
{
    if (*ts - time_menu < BUTTON_DEBOUNCE_MS)
        return false;

    if ((PINB & bit(PINB1)) > 0)
      return false;
    
    time_menu = *ts;
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