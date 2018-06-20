/* generated thread header file - do not edit */
#ifndef PWM_CTRL_THREAD_H_
#define PWM_CTRL_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void pwm_ctrl_thread_entry(void);
#else 
extern void pwm_ctrl_thread_entry(void);
#endif
#ifdef __cplusplus
extern "C"
{
#endif
extern TX_EVENT_FLAGS_GROUP g_event_change_pattern;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* PWM_CTRL_THREAD_H_ */
