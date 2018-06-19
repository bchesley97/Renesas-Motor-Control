/***********************************************************************************************************************
 * Copyright [2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * This file is part of Renesas SynergyTM Software Package (SSP)
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : r_motor.c
 * Description  : Common Motor control functions and global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "r_motor_api_changed.h"
#include "r_gpt.h"
#include "bsp_api.h"
#include "common_data.h"

volatile uint32_t pwm_cycle_cntr = 0;
void (*sf_callback)(void) = NULL;
bool irq_handler_set = false;
int32_t     g_motor_count = 0;
motor_instance_t const *g_motors[16];
int16_t     g_pwm_timer_count;
uint32_t pwm_cycle_state_thresh = 25; //arbitrary value (for now) controlling how many pwm cycles till the trap pattern is changed
uint32_t pwm_inner_loop_cntr = 0;
int32_t pwm_velocity = 0;

uint8_t pattern_index = 1;

extern mtr_added_ctrl_t *p_mtr_pattern_ctrl;
extern trap_phase_state_t trap_pattern[6];
extern phase_pin_ctrl_t *p_pins_ctrl;
extern phase_pin_ctrl_t pins_ctrl[6];

/* Interrupt handler for high speed motor control loops */
void pwm_counter_overflow (void);
void pwm_counter_overflow (void)
{
    /* Save context if RTOS is used */
    SF_CONTEXT_SAVE

    ssp_vector_info_t * p_vector_info = NULL;
    IRQn_Type irq = R_SSP_CurrentIrqGet();
    R_SSP_VectorInfoGet(irq, &p_vector_info);
    motor_instance_t *trap_motor = g_motors[1]; //hardcode the motor that has a trapezoidal commutation scheme

//    if (sf_callback != NULL)
//    {
//        /* If there is a motor owner - call its callback */
//        sf_callback();
//    }
//    else
//    {
//        /* If the motor modules are stand alone - call their control function */
//        for (int32_t i = 0; i < g_motor_count; i++)
//        {
//            if (g_motors[i]->p_api != NULL)
//                g_motors[i]->p_api->control(g_motors[i]->p_ctrl);
//        }
//    }
    /* Keep track of the number of PWM interrupts (control cycles) */
    pwm_cycle_cntr++;

    if(pwm_cycle_cntr >= p_mtr_pattern_ctrl->vel_accel.velocity && p_mtr_pattern_ctrl->ctrl_type == OPEN_LOOP_CONTROL)
    {
        pwm_cycle_cntr = 0;

        //update pointer holding data about pin levels for specific trapezoidal commutation
        p_pins_ctrl++;
        if(p_pins_ctrl > &pins_ctrl[5])
        {
            p_pins_ctrl = &pins_ctrl[0]; //reset pointer to beginning of pattern array
        }


        //get values from memory so block timers can be updated faster (promote synchronicity of timer updates)
        pin_duty_state pins_u = p_pins_ctrl->u_phase_pins.pins_duty_reg;
        pin_duty_state pins_v = p_pins_ctrl->v_phase_pins.pins_duty_reg;
        pin_duty_state pins_w = p_pins_ctrl->w_phase_pins.pins_duty_reg;

        /*** Update timer pins output ***/
        //update U timer
        trap_motor->p_ctrl->p_gpt_u->GTUDDTYC = pins_u;   //changing the OADTY/OBDTY bits allows for synchronous changing of the timer's pin outputs at a timer underflow

        //update V timer
        trap_motor->p_ctrl->p_gpt_v->GTUDDTYC = pins_v;

        //update W timer
        trap_motor->p_ctrl->p_gpt_w->GTUDDTYC = pins_w;

    }

    /** Clear pending IRQ to make sure it doesn't fire again after exiting */
    R_BSP_IrqStatusClear(irq);

    /* Restore context if RTOS is used */
    SF_CONTEXT_RESTORE
    if (g_motors[0]->p_ctrl != NULL)
        g_pwm_timer_count = (int16_t)g_motors[0]->p_ctrl->p_gpt_u->GTCNT;
} /* End of function gpt_counter_overflow_common */



