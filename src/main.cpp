#include <Arduino.h>
#include "Wire.h"
#include "EEPROM.h"
#include "LiquidCrystal_PCF8574.h"

#include "project.h"
#include "fsm.h"
#include "relay.h"
#include "sensor.h"
#include "display.h"

/* DISPLAY GLOBAL DATA */
static display_params_t display_data = { 0 };
static bool sensor_limit_changed = false;
static bool sensor_limit_unsaved = false;

/* APP MODE MANAGEMENT PRIVATE VARIABLES */

static volatile unsigned long mode_button_ts = 0;
static void mode_isr();
static void mode_menuconfig_setup(int state);
static void mode_menuconfig_loop(int state);
static void mode_counter_setup(int state);
static void mode_counter_loop(int state);

/**
 * SCREEN CONTENT
*/
static const char * fmt_str_l16 = "%16s";
static const char * fmt_u16_l4 = "%4u";
static const char * fmt_u32_l8 = "%8u";

static const char * factory_reset_screen = "FACTORY RESET";
static const char * counter_screen[2] = {
  "ACCION  | C:    ",
  "        | D:    "
};
static const char * menuconfig_screen[2] = {
  "CONFIGURACION   ",
  "> Divisor:      "
};

static display_frame_t frame_counter_screen[2] = {
  {
    .type = DISPLAY_TYPE_STR,
    .data = (void*) counter_screen[0],
    .format = fmt_str_l16,
    .pos_x = 0,
    .pos_y = 0
  },
  {
    .type = DISPLAY_TYPE_STR,
    .data = (void*) counter_screen[1],
    .format = fmt_str_l16,
    .pos_x = 0,    
    .pos_y = 1
  }
};

static display_frame_t frame_menuconfig_screen[2] = {
  {
    .type = DISPLAY_TYPE_STR,
    .data = (void*) menuconfig_screen[0],
    .format = fmt_str_l16,
    .pos_x = 0,
    .pos_y = 0
  },
  {
    .type = DISPLAY_TYPE_STR,
    .data = (void*) menuconfig_screen[1],
    .format = fmt_str_l16,
    .pos_x = 0,    
    .pos_y = 1
  }
};

static display_frame_t frame_limit = {
  .type = DISPLAY_TYPE_U16,
  .data = &(display_data.limit),
  .format = fmt_u16_l4,
  .pos_x = 12,
  .pos_y = 1
};

static display_frame_t frame_counter = {
  .type = DISPLAY_TYPE_U16,
  .data = &(display_data.counter),
  .format = fmt_u16_l4,
  .pos_x = 12,
  .pos_y = 0
};

static display_frame_t frame_actions = {
  .type = DISPLAY_TYPE_U32,
  .data = &(display_data.actions),
  .format = fmt_u32_l8,
  .pos_x = 0,
  .pos_y = 1
};

/**
 * MENU BUTTON IO FUNCTIONS
 */

static void button_down(uint8_t port)
{
  sensor_limit_decrease();
  sensor_limit_changed = true;
}

static void button_up(uint8_t port)
{
  sensor_limit_increase();
  sensor_limit_changed = true;
}

static void mode_isr()
{
  DEBOUNCE_ISR(mode_button_ts, BUTTON_DEBOUNCE_MS);

  /* CHECK AND MOVE FROM CURRENT FSM STATE */

  if (fsm_switch_from_isr(MODE_MENUCONFIG_LOOP, MODE_COUNTER_SETUP) == 0)
  {
    /* SUCCESFUL, EXIT */
    return;
  }

  /* ELSE CHECK LAST ONE */

  fsm_switch_from_isr(MODE_COUNTER_LOOP, MODE_MENUCONFIG_SETUP);
}

static inline void mode_init()
{
  DDRD &= ~(bit(DD3)); /* set PD3 as input (D3 ext int 1) */
  PORTD |= bit(PORT3); /* set PD3 as pullup (D3 ext int 1) */
  attachInterrupt(1, mode_isr, FALLING);

  fsm_add_state(MODE_MENUCONFIG_SETUP, mode_menuconfig_setup);
  fsm_add_state(MODE_MENUCONFIG_LOOP, mode_menuconfig_loop);
  fsm_add_state(MODE_COUNTER_SETUP, mode_counter_setup);
  fsm_add_state(MODE_COUNTER_LOOP, mode_counter_loop);

  fsm_start(INITIAL_MODE);
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

    display_frame_t frame = {
      .type = DISPLAY_TYPE_STR,
      .data = (void*) factory_reset_screen,
      .format = fmt_str_l16,
      .pos_x = 0,
      .pos_y = 0
    };

    display_frame(&frame);

    delay(1000);
  }
  
  /* Initialize sensor (digital input) */

  sensor_init();
  
  /* Initialize relayu (digital output) */

  relay_init();

  /* Initialize app mode switch button and callbacks */

  mode_init(); 

  /* Update variables */
  
  display_data.limit = sensor_limit_get();

}

/**
 * GENERAL LOOP
 */
void loop()
{
  /* STATE MACHINE LOOP*/

  fsm_loop();

  /* IO CHECK LOOP */

  relay_loop();
}

/**
 * PROGRAMMING MODE SETUP
 */
static void mode_menuconfig_setup(int state)
{
  /* Activate ISR call from menu buttons */

  menu_buttons_activate();

  /* Print display with current display_data */

  display_frame(&frame_menuconfig_screen[0]);
  display_frame(&frame_menuconfig_screen[1]);
  display_frame(&frame_limit);

  /* Switch to next state */

  fsm_switch_to(MODE_MENUCONFIG_LOOP);
}

/**
 * PROGRAMMING MODE LOOP
 */
static void mode_menuconfig_loop(int state)
{
  /* Run menu button callback functions */

  menu_button_loop();

  /* Continue if changes to limit data was made, if not return */

  if (sensor_limit_changed == false)
    return;

  /* Update display */
  sensor_limit_changed = false;
  sensor_limit_unsaved = true;

  display_data.limit = sensor_limit_get();
  
  display_frame(&frame_limit);
}

/**
 * COUNTER MODE SETUP
 */
static void mode_counter_setup(int state)
{
  /* Deactivate ISR call from menu buttons */

  menu_buttons_deactivate();

  /* Save values if required */

  if (sensor_limit_unsaved == true)
  {
    sensor_limit_unsaved = false;
    sensor_save_settings();
  }

  /* Print display with current display_data */

  display_frame(&frame_counter_screen[0]);
  display_frame(&frame_counter_screen[1]);
  display_frame(&frame_actions);
  display_frame(&frame_counter);
  display_frame(&frame_limit);

  /* Move to next state */

  fsm_switch_to(MODE_COUNTER_LOOP);
}

/**
 * COUNTER MODE LOOP
 */
static void mode_counter_loop(int state)
{
  /* Continue if new data available, if not return */

  if (sensor_data_available() == false)
    return;

  /* Update and display display_data */

  display_data.counter = sensor_counter_get();
  display_data.actions = sensor_actions_get();
 
  display_frame(&frame_actions);
  display_frame(&frame_counter);
  display_frame(&frame_limit);
}

/**
 * ISR CALLBACK FUNCTIONS
 */

/* PIN CHANGE PORT B ISR (MENU CONTROL) */
ISR(PCINT0_vect)
{
  unsigned long ts = millis();
  menu_button_isr(&ts);
}
