#ifndef F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F
#define F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F

#include <Arduino.h>

typedef struct display_params {
    uint16_t limit;
    uint16_t counter;
    uint32_t actions;
} display_params_t;

void display_init();
void display_clear();
void display_line(const char * str, uint8_t row);
void display_num_u16(uint16_t number, uint8_t row);
void display_main_screen_print(display_params_t * params);
void display_main_screen_update(display_params_t * params);
void display_second_screen_print(display_params_t * params);
void display_second_screen_update(display_params_t * params);

void menu_button_init();
bool menu_button_config(uint8_t button, void (*f)(uint8_t port));
void menu_button_loop();
void menu_button_isr();
void menu_button_isr_ts(unsigned long * ts);

void menu_buttons_activate();
void menu_buttons_deactivate();


#endif /* F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F */
