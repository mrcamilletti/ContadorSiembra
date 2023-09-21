#ifndef E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6
#define E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6

#include <Arduino.h>

void sensor_init();

void sensor_limit_set(uint16_t limit);
void sensor_save_settings();
void sensor_limit_increase();  
void sensor_limit_decrease();
uint16_t sensor_limit_get();

uint16_t sensor_counter_get();
uint32_t sensor_actions_get();

#endif /* E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6 */
