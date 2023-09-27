#ifndef F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F
#define F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F

#include <Arduino.h>

typedef struct {
    uint16_t limit;
    uint16_t counter;
    uint32_t actions;
} display_params_t;

typedef enum {
    DISPLAY_TYPE_U8,
    DISPLAY_TYPE_I8,
    DISPLAY_TYPE_U16,
    DISPLAY_TYPE_I16,
    DISPLAY_TYPE_U32,
    DISPLAY_TYPE_I32,
    DISPLAY_TYPE_STR,

    DISPLAY_TYPE_MAX
} display_type_t;

typedef struct {   
    display_type_t type;
    void * data;
    const char * format;
    uint8_t pos_x;
    uint8_t pos_y;
} display_frame_t;

void display_init();
void display_clear();
bool display_frame(display_frame_t * frame);

void menu_button_init();
bool menu_button_config(uint8_t button, void (*f)(uint8_t port));
void menu_button_loop();
void menu_button_isr(const unsigned long * ts);

void menu_buttons_activate();
void menu_buttons_deactivate();


#endif /* F2866C0B_36CA_4A14_A5AB_70FBB5F2B35F */
