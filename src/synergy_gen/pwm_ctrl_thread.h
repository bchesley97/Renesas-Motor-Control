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
#include "r_icu.h"
#include "r_external_irq_api.h"
#include "sf_external_irq.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* External IRQ on ICU Instance. */
extern const external_irq_instance_t g_external_irq0;
#ifndef external_irq_callback
void external_irq_callback(external_irq_callback_args_t *p_args);
#endif
/** SF External IRQ on SF External IRQ Instance. */
extern const sf_external_irq_instance_t g_sf_external_irq0;
extern TX_EVENT_FLAGS_GROUP g_event_change_pattern;
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* PWM_CTRL_THREAD_H_ */
