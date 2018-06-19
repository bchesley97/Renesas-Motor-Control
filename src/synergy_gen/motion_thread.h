/* generated thread header file - do not edit */
#ifndef MOTION_THREAD_H_
#define MOTION_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus 
extern "C" void motion_thread_entry(void);
#else 
extern void motion_thread_entry(void);
#endif
#include "r_encoder_api.h"
#include "r_encoder_gpt.h"
#include "r_motor_api.h"
#include "r_motor_bldc.h"
#include "sf_motion.h"
#ifdef __cplusplus
extern "C"
{
#endif
/** Encoder on GPT instance */
extern encoder_instance_t g_r_encoder_gpt2;
/** BLDC Motor on Motor instance */
#define TIMER_ON_GPT_CALLBACK_USED_g_r_motor_bldc2 (1)
/* Motion Framework instance */
extern sf_motion_instance_t g_sf_motion2;
#ifdef NULL
#define MOTION_CALLBACK_USED_g_sf_motion2 (0)
#else
#define MOTION_CALLBACK_USED_g_sf_motion2 (1)
#endif

#if MOTION_CALLBACK_USED_g_sf_motion2
/** Declaration of user callback function. This function MUST be defined in the user application.*/
void NULL(sf_motion_callback_args_t *p_args);
#endif
/** Encoder on GPT instance */
extern encoder_instance_t g_r_encoder_gpt1;
/** BLDC Motor on Motor instance */
#define TIMER_ON_GPT_CALLBACK_USED_g_r_motor_bldc1 (1)
/* Motion Framework instance */
extern sf_motion_instance_t g_sf_motion1;
#ifdef NULL
#define MOTION_CALLBACK_USED_g_sf_motion1 (0)
#else
#define MOTION_CALLBACK_USED_g_sf_motion1 (1)
#endif

#if MOTION_CALLBACK_USED_g_sf_motion1
/** Declaration of user callback function. This function MUST be defined in the user application.*/
void NULL(sf_motion_callback_args_t *p_args);
#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* MOTION_THREAD_H_ */
