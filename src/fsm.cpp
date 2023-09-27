#include <errno.h>
#include "fsm.h"

static uint32_t fsm_declared_states;
static fsm_function_t fsm_states[FSM_STATES_MAX];
fsm_state_t fsm_current_state;

uint8_t fsm_started = 0;

int fsm_add_state(fsm_state_t state, fsm_function_t cb)
{
    if (!(state < FSM_STATES_MAX))
        return -ENOMEM;

    if (fsm_declared_states & (1 << state))
        return -EINVAL;    
    
    fsm_declared_states |= (1 << state);
    fsm_states[state] = cb;

    return 0;
}

int fsm_switch_to(fsm_state_t state)
{
    if (!(state < FSM_STATES_MAX))
        return -ENOMEM;

    if (!(fsm_declared_states & (1 << state)))
        return -EINVAL;

    fsm_current_state = state;

    return 0;
}

int fsm_switch_from_isr(fsm_state_t from_state, fsm_state_t to_state)
{
    if (!(from_state == fsm_current_state))
        return -EAGAIN;
    
    if (!(to_state < FSM_STATES_MAX))
        return -ENOMEM;

    if (!(fsm_declared_states & (1 << to_state)))
        return -EINVAL;

    fsm_current_state = to_state;

    return 0;
}

int fsm_start(fsm_state_t state)
{
    if (fsm_declared_states == 0)   
        return -EINVAL;
    
    if (!(state < FSM_STATES_MAX))
        return -ENOMEM;

    if (!(fsm_declared_states & (1 << state)))
        return -EINVAL;
    
    fsm_started = 1;
    fsm_current_state = state;
    
    return 0;
}

int fsm_loop()
{
    if(!fsm_started)
        return -EINVAL;

    fsm_states[fsm_current_state](fsm_current_state);

    return 0;
}