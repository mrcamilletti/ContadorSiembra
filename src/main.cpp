#include <Arduino.h>
#include "project.h"
#include "Wire.h"
#include "LiquidCrystal_PCF8574.h"

#include "relay.h"
#include "sensor.h"
#include "display.h"

static volatile unsigned long time_mode = 0;

static enum app_mode global_mode = INITIAL_MODE;

static uint16_t contador_old = 0;
static uint16_t limite_old = 1;

static void mode_isr();

static bool set_next_state(enum app_mode next_mode);

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
  decrease_sensor_limit();
}

static void button_up(uint8_t port)
{
  increase_sensor_limit();
}


static void mode_isr()
{
  DEBOUNCE_ISR(time_mode, BUTTON_DEBOUNCE_MS);

  switch (global_mode)
  {
  case MODE_PROGRAM_LOOP:
    global_mode = MODE_COUNTER_SETUP;
    break;
  case MODE_COUNTER_LOOP:
    global_mode = MODE_PROGRAM_SETUP;
    break;

  default:
    break;
  }
}

static void mode_init()
{
  DDRD &= ~(bit(DD3)); // set PD3 as input (D3 ext int 1)
  PORTD |= bit(PORT3); // set PD3 as pullup (D3 ext int 1)
  attachInterrupt(1, mode_isr, FALLING);
}

static bool set_next_state(enum app_mode next_mode)
{
  if (!(next_mode < MODE_MAX))
    return false;

  global_mode = next_mode;

  return true;
}

static inline void run_loop()
{  
  mode_loop_f[global_mode]();
}


/**
 * GENERAL SETUP
 */
void setup()
{
  Wire.begin();

//  Serial.begin(9600);
//  while (!Serial);

  // find_i2c_devices();

  init_display();
  init_sensor();
  init_relay();

  menu_button_config(0, button_down);
  menu_button_config(1, button_up);

  mode_init();  
}

/**
 * GENERAL LOOP
 */
void loop()
{
  /* STATE MACHINE LOOP*/
  run_loop();

  /* IO CHECK LOOP */
  relay_loop();
}

/**
 * PROGRAMMING MODE SETUP
 */
static inline void mode_program_setup()
{
  menu_buttons_activate();
  
  display_params_t data = {
    .limit = limite_old
  };
  
  display_second_screen_print(&data);
  
  set_next_state(MODE_PROGRAM_LOOP);
}

/**
 * PROGRAMMING MODE LOOP
 */
static void mode_program_loop()
{
  menu_button_loop();  

  uint16_t limite = get_sensor_limit();    
  
  if (limite == limite_old)
    return;
  
  limite_old = limite;

  display_params_t data = {
    .limit = limite
  };
  
  display_second_screen_print(&data);  
}

/**
 * COUNTER MODE SETUP
 */
static void mode_counter_setup()
{
  menu_buttons_deactivate();
 
  display_params_t data = {
    .limit = get_sensor_limit(),
    .counter = contador_old,
    .actions = get_sensor_actions()    
  };

  display_main_screen_print(&data);
  
  set_next_state(MODE_COUNTER_LOOP);
}

/**
 * COUNTER MODE LOOP
 */
static void mode_counter_loop()
{  
  uint16_t contador = get_sensor_counter();

  if (contador_old == contador)
    return;

  contador_old = contador;

  display_params_t data = {
    .limit = get_sensor_limit(),
    .counter = contador,
    .actions = get_sensor_actions()    
  };

  display_main_screen_update(&data);
}

/**
 * ISR CALLBACK FUNCTIONS
 */

/* PIN CHANGE PORT B ISR (MENU CONTROL) */
unsigned long pcint_ts;
ISR(PCINT0_vect)
{
  menu_button_isr(); 
}
