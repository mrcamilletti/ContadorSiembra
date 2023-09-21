#include <Arduino.h>
#include "Wire.h"
#include "EEPROM.h"
#include "LiquidCrystal_PCF8574.h"

#include "project.h"
#include "relay.h"
#include "sensor.h"
#include "display.h"

static enum app_mode mode = INITIAL_MODE;

static volatile unsigned long mode_button_ts = 0;

static uint16_t contador_old = 0;
static uint16_t limite_old;

static display_params_t display_data = { 0 };

static void mode_isr();

static bool mode_next(enum app_mode next_mode);

static void mode_program_setup();
static void mode_program_loop();
static void mode_counter_setup();
static void mode_counter_loop();

static void (*mode_loop_f[MODE_MAX])(void) = {
    mode_program_setup,
    mode_program_loop,
    mode_counter_setup,
    mode_counter_loop};

/**
 * MENU BUTTON IO FUNCTIONS
 */

static void button_down(uint8_t port)
{
  sensor_limit_decrease();
}

static void button_up(uint8_t port)
{
  sensor_limit_increase();
}

static void mode_isr()
{
  DEBOUNCE_ISR(mode_button_ts, BUTTON_DEBOUNCE_MS);

  switch (mode)
  {
  case MODE_PROGRAM_LOOP:
    mode = MODE_COUNTER_SETUP;
    break;
  case MODE_COUNTER_LOOP:
    mode = MODE_PROGRAM_SETUP;
    break;

  default:
    break;
  }
}

static inline void mode_init()
{
  DDRD &= ~(bit(DD3)); // set PD3 as input (D3 ext int 1)
  PORTD |= bit(PORT3); // set PD3 as pullup (D3 ext int 1)
  attachInterrupt(1, mode_isr, FALLING);
}

static bool mode_next(enum app_mode next)
{
  if (!(next < MODE_MAX))
    return false;

  mode = next;

  return true;
}

static inline void mode_loop()
{
  mode_loop_f[mode]();
}

/**
 * GENERAL SETUP
 */
void setup()
{
  /* Initialize instances */

  Wire.begin();

#if CONFIG_USE_SERIAL_OUTPUT
  Serial.begin(9600);
  while (!Serial);
#endif

#if CONFIG_CHECK_I2C_DEVICES
  find_i2c_devices();
#endif

  /* Configure menu buttons and function callbacks */
  menu_button_config(0, button_down);
  menu_button_config(1, button_up);

  /* Initialize display (I2C) */

  display_init();

  /* Check if user requested a factory reset (UP & DOWN buttons pressed) */

  if ((PINB & (bit(PIN0) | bit(PIN1))) == 0)
  {
    EEPROM.begin();
    EEPROM.write(0, 0xFF);
    EEPROM.write(1, 0xFF);

    display_line("FACTORY RESET", 0);
    delay(1000);
  }

  /* Initialize sensor (digital input) */

  sensor_init();
  
  /* Initialize relayu (digital output) */

  relay_init();

  /* Initialize app mode switch button and callbacks */

  mode_init();

  /* Update variables */

  limite_old = sensor_limit_get();
  display_data.limit = limite_old;

}

/**
 * GENERAL LOOP
 */
void loop()
{
  /* STATE MACHINE LOOP*/

  mode_loop();

  /* IO CHECK LOOP */

  relay_loop();
}

/**
 * PROGRAMMING MODE SETUP
 */
static void mode_program_setup()
{
  /* Activate ISR call from menu buttons */

  menu_buttons_activate();

  /* Print display with current display_data */

  display_second_screen_print(&display_data);

  /* Switch to next state */

  mode_next(MODE_PROGRAM_LOOP);
}

/**
 * PROGRAMMING MODE LOOP
 */
static void mode_program_loop()
{
  /* Run menu button callback functions */

  menu_button_loop();

  /* Continue if changes to parameters were made, if not return */

  uint16_t limite = sensor_limit_get();

  if (limite == limite_old)
    return;

  limite_old = limite;

  /* Update display */

  display_data.limit = limite;
  display_second_screen_update(&display_data);
}

/**
 * COUNTER MODE SETUP
 */
static void mode_counter_setup()
{
  /* Deactivate ISR call from menu buttons */

  menu_buttons_deactivate();

  /* Save values */

  sensor_save_settings();

  /* Print display with current display_data */

  display_main_screen_print(&display_data);

  /* Move to next state */

  mode_next(MODE_COUNTER_LOOP);
}

/**
 * COUNTER MODE LOOP
 */
static void mode_counter_loop()
{
  /* Continue if counter changes, if not return */

  uint16_t contador = sensor_counter_get();

  if (contador_old == contador)
    return;

  /* Update reference value */

  contador_old = contador;

  /* Update and display display_data */

  display_data.counter = contador;
  display_data.actions = sensor_actions_get();

  display_main_screen_update(&display_data);
}

/**
 * ISR CALLBACK FUNCTIONS
 */

/* PIN CHANGE PORT B ISR (MENU CONTROL) */
ISR(PCINT0_vect)
{
  menu_button_isr();
}
