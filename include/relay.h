#ifndef A9ECFA3D_F84C_4CDF_865A_89C51A258AD4
#define A9ECFA3D_F84C_4CDF_865A_89C51A258AD4

#include <Arduino.h>

void relay_init();
void relay_on();
void relay_on_from_isr(unsigned long * ts);
void relay_off();
void relay_loop();
uint8_t relay_status();


#endif /* A9ECFA3D_F84C_4CDF_865A_89C51A258AD4 */
