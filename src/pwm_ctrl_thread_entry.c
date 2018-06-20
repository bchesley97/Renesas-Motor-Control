#include "pwm_ctrl_thread.h"
#include "r_motor_api_changed.h"

extern mtr_added_ctrl_t *p_mtr_pattern_ctrl;

bool pwm_pattern_update = 0; //global variable. this variable is used in the ISR to determine when to update the pwm pattern (change phase states)

extern phase_pin_ctrl_t pins_ctrl[];
extern phase_pin_ctrl_t *p_pins_ctrl;

/* PWM Control Thread entry function */
void pwm_ctrl_thread_entry(void)
{

    tx_thread_sleep (5); //update every 10 ms, less for closed loop control (faster speeds)

    while (1)
    {

        //check which type of control to perform
        if (p_mtr_pattern_ctrl->ctrl_type == OPEN_LOOP_CONTROL)
        {
            //check to make sure the pwm update has registered with the ISR. used as a synchronizing tool since cant use semaphores in an ISR
            if (p_mtr_pattern_ctrl->vel_accel.velocity > CONTROL_SWITCH_VELOCITY)
            {
                //p_mtr_pattern_ctrl->vel_accel.velocity -= p_mtr_pattern_ctrl->vel_accel.acceleration; //add acceleration to the velocity (subtracting because these are in terms of pwm cycles)
                tx_thread_sleep (100); //update every 500 ms

            }
            tx_thread_sleep (1); //update every 10 ms, less for closed loop control (faster speeds)

//            else if(p_pins_ctrl == &pins_ctrl[5]) //only switch control to closed loop when the pattern is the last in the sequence
//            {
//                R_DTC->DTCST = 1; //start DTC
//                p_mtr_pattern_ctrl->ctrl_type = CLOSED_LOOP_CONTROL; //hit user defined unit for switching to closed loop control, start PID control
//
//                tx_thread_sleep (1); //update every 10 ms, less for closed loop control (faster speeds)
//
//            }

        }
        else
        {
            //closed loop control
           // g_sf_external_irq0.p_api->wait(g_sf_external_irq0.p_ctrl, TX_WAIT_FOREVER); //interrupt is firing, DTC not properly initialized
            tx_thread_sleep (1); //update every 10 ms, less for closed loop control (faster speeds)

        }

    }
}

