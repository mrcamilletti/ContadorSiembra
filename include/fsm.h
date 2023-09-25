#ifndef C7FA984F_986E_411F_9210_A17B66A370C3
#define C7FA984F_986E_411F_9210_A17B66A370C3

#include <stdint.h>

#define FSM_STATES_MAX  4
#define FSM_STATES_TYPEDEF  uint8_t

typedef void (*fsm_function_t)(int state);
typedef FSM_STATES_TYPEDEF fsm_state_t;


int fsm_add_state(fsm_state_t state, fsm_function_t cb);
int fsm_switch_to(fsm_state_t state);
int fsm_switch_from_isr(fsm_state_t from_state, fsm_state_t to_state);
int fsm_start(fsm_state_t state);
int fsm_loop();

#endif /* C7FA984F_986E_411F_9210_A17B66A370C3 */
