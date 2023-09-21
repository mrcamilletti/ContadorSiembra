#include <Arduino.h>
#include "EEPROM.h"

#include "project.h"
#include "sensor.h"
#include "relay.h"


static uint16_t _limite = 1;
static uint16_t _contador = 0;
static uint32_t _acciones = 0;

static unsigned long time_sensor = 0;
static void sensor_isr()
{
  DEBOUNCE_ISR(time_sensor, SENSOR_DEBOUNCE_MS);

  _contador++;
  if (_contador == _limite)
  {
    _contador = 0;    
    _acciones++;
    relay_on_from_isr(&time_sensor);
  }
}

void sensor_init()
{
  EEPROM.begin();
  EEPROM.get(CONFIG_LIMIT_EEPROM_ADDR, _limite);
  if (_limite == ((uint16_t)-1))
  {
    _limite = 1;
  }

  DDRD &= ~(bit(DD2)); // set PD3 as input (D3 ext int 1)
  PORTD |= bit(PORT2); // set PD3 as pullup (D3 ext int 1)
  attachInterrupt(0, sensor_isr, FALLING);
}

void sensor_save_settings()
{
  EEPROM.put(CONFIG_LIMIT_EEPROM_ADDR, _limite);
}

void sensor_limit_set(uint16_t limit)
{
    if (limit == 0) 
        return;

    _limite = limit;
}

void sensor_limit_increase()
{
    if (_limite < MAX_COUNTER_LIMIT)
    {
      _limite++;
    }
}

void sensor_limit_decrease()
{
    if (_limite > 1)
    {
      _limite--;
    }
}


uint16_t sensor_limit_get()
{
  return _limite;
}


uint16_t sensor_counter_get()
{
    return _contador;
}

uint32_t sensor_actions_get()
{
    return _acciones;
}