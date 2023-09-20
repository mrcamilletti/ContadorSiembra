#ifndef E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6
#define E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6

#include <Arduino.h>

void init_sensor();

void set_sensor_limit(uint16_t limit);
void save_sensor_limit();
void increase_sensor_limit();  
void decrease_sensor_limit();
uint16_t get_sensor_limit();

uint16_t get_sensor_counter();
uint32_t get_sensor_actions();

#endif /* E87ADF19_F27F_4ABF_8C81_A20B1CC7D5E6 */
