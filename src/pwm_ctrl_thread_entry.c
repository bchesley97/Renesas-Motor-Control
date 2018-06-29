#include "pwm_ctrl_thread.h"
#include "r_motor_api_changed.h"

extern mtr_added_ctrl_t *p_mtr_pattern_ctrl;

bool pwm_pattern_update = 0; //global variable. this variable is used in the ISR to determine when to update the pwm pattern (change phase states)

extern phase_pin_ctrl_t pins_ctrl[];
extern phase_pin_ctrl_t *p_pins_ctrl;
extern motor_instance_t const *g_motors[16];
/* PWM Control Thread entry function */
void pwm_ctrl_thread_entry(void)
{

    tx_thread_sleep (5); //update every 10 ms, less for closed loop control (faster speeds)

    uint32_t acceleration_counter = 0;
    uint8_t pwm_duty_cycle = 1; //in percent, this is the starting value

    while (1)
    {

        //check which type of control to perform
        if (p_mtr_pattern_ctrl->ctrl_type == OPEN_LOOP_CONTROL)
        {

            if (p_mtr_pattern_ctrl->vel_accel.velocity > CONTROL_SWITCH_VELOCITY)
            {
                p_mtr_pattern_ctrl->vel_accel.velocity -= p_mtr_pattern_ctrl->vel_accel.acceleration; //add acceleration to the velocity (subtracting because these are in terms of pwm cycles)

//                acceleration_counter++;
//                if(acceleration_counter > 9)
//                {
//                    acceleration_counter = 0;
//                    change_pwm_duty(pwm_duty_cycle, g_motors[1]->p_ctrl);
//                    pwm_duty_cycle+=3;
//
//                }


                tx_thread_sleep (2); //update every 500 ms
            }

            else if(p_pins_ctrl == &pins_ctrl[5]) //only switch control to closed loop when the pattern is the last in the sequence
            {
                R_DTC->DTCST = 1; //start DTC
                //p_mtr_pattern_ctrl->ctrl_type = CLOSED_LOOP_CONTROL; //hit user defined unit for switching to closed loop control, start PID control

                tx_thread_sleep (1); //update every 10 ms, less for closed loop control (faster speeds)

            }

        }
        else
        {
            //closed loop control
            tx_thread_sleep (1); //update every 10 ms, less for closed loop control (faster speeds)

        }

    }
}

