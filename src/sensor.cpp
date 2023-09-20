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

void init_sensor()
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

void save_sensor_limit()
{
  EEPROM.put(CONFIG_LIMIT_EEPROM_ADDR, _limite);
}

void set_sensor_limit(uint16_t limit)
{
    if (limit == 0) 
        return;

    _limite = limit;
}

void increase_sensor_limit()
{
    if (_limite < MAX_COUNTER_LIMIT)
    {
      _limite++;
    }
}

void decrease_sensor_limit()
{
    if (_limite > 1)
    {
      _limite--;
    }
}


uint16_t get_sensor_limit()
{
  return _limite;
}


uint16_t get_sensor_counter()
{
    return _contador;
}

uint32_t get_sensor_actions()
{
    return _acciones;
}