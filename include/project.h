#ifndef F50C7B16_3BD4_460B_A67A_855EA3A986F2
#define F50C7B16_3BD4_460B_A67A_855EA3A986F2

#define CONFIG_LIMIT_EEPROM_ADDR 0

#define MODE_SWITCH_PIN 4

#define MAX_COUNTER_LIMIT 500

#define MENU_BUTTON_UP 0
#define MENU_BUTTON_DN 1
#define RELAY_PIN 2

enum app_mode
{
  MODE_MENUCONFIG_SETUP,
  MODE_MENUCONFIG_LOOP,
  MODE_COUNTER_SETUP,
  MODE_COUNTER_LOOP,

  MODE_MAX
};

#define INITIAL_MODE  MODE_COUNTER_SETUP

#define BUTTON_DEBOUNCE_MS 200
#define SENSOR_DEBOUNCE_MS 500
#define RELAY_WINDOW_MS 500

#define DEBOUNCE_CMP(ts, var, time)   \
  do                              \
  {                               \
    if (ts - var < time)          \
      return;                     \
    var = ts;                     \
  } while (0)

#define DEBOUNCE_ISR(var, time)   \
  do                              \
  {                               \
    unsigned long _ts = millis(); \
    if (_ts - var < time)         \
      return;                     \
    var = _ts;                    \
  } while (0)

#define DEBOUNCE_DO(var, time, todo) \
  do                                 \
  {                                  \
    unsigned long _ts = millis();    \
    if (_ts - var < time)            \
      continue;                      \
    var = _ts;                       \
    todo;                            \
  } while (0)

#endif /* F50C7B16_3BD4_460B_A67A_855EA3A986F2 */
