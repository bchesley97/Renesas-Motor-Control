
#ifndef DRV_MOTOR_API_CHANGED_H
#define DRV_MOTOR_API_CHANGED_H

/*******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup TIMER_API Timer Interface
 * @brief Interface for timer functions.
 *
 * @section MOTOR_API_SUMMARY Summary
 * The general motor interface provides methods and configuration settings required for the control of different types
 * of motors such as BLDC, DC, ACIM or Stepper
 *
 * Implemented by:
 * - @ref MOTOR_BLDC
 * - @ref MOTOR_DC
 * - @ref MOTOR_ACIM
 * - @ref MOTOR_STEPPER
 *
 * See also:
 * @ref ENCODER_API
 *
 * Related SSP architecture topics:
 *  - @ref ssp-interfaces
 *  - @ref ssp-predefined-layers
 *  - @ref using-ssp-modules
 *
 * Timer Interface description: @ref HALTimerInterface
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "r_motor_api.h"
/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/* Enum added for trapezoidal commutation, all three possible states the phases can be in */
typedef enum e_phase_status
{
    PSTAT_PWM = 0,
    PSTAT_HIGH,
    PSTAT_LOW
}phasing_status_t;

typedef enum e_motor_running_state
{
    OPEN_LOOP_CONTROL = 0, //low speed open loop control when the speed is slow and back EMF isnt large enough to discern different phasing states
    CLOSED_LOOP_CONTROL //when speed is high enough to use back EMF sensing to discern phasing states
}motor_running_state;

//this enum controls the value of the OmTDY bits of the GTUDDTYC. This allows the pin value to change
//typedef enum e_phase_reg_states
//{
//    PIN_DUTY_PWM         = 0,
//    PIN_DUTY_100         = 0b11,
//    PIN_DUTY_0           = 0b10
//}pin_duty_state;

//this enum is different than the above enum as this will access the entire duty cycle register rather than having a
typedef enum e_phase_reg_states
{
    PIN_DUTY_PWM         = 0x00000001,  //1 in bit 0 to keep UD bit = 1 (up counting)
    PIN_DUTY_100         = 0x03020001,
    PIN_DUTY_0           = 0x02020001
}pin_duty_state;

typedef struct pin_ctrl
{
    pin_duty_state pins_duty_reg;

}pin_ctrl_t;

typedef struct phase_pin_ctrl
{
    pin_ctrl_t u_phase_pins;
    pin_ctrl_t v_phase_pins;
    pin_ctrl_t w_phase_pins;
}phase_pin_ctrl_t;

typedef struct phasing_state
{
    phasing_status_t u_state;
    phasing_status_t v_state;
    phasing_status_t w_state;
}trap_phase_state_t;

typedef struct vel_accel
{
    int32_t velocity;
    int32_t acceleration;
}mtr_pattern_vel_accel_t;



typedef struct mtr_added_ctrl
{
    trap_phase_state_t * p_trap_pattern; //set this equal to &trap_pattern[0] in initialization
    mtr_pattern_vel_accel_t vel_accel;
    motor_running_state ctrl_type;
}mtr_added_ctrl_t;

/** Global variable for pattern pointer since cant access header file to change the ctrl struct **/


/*** Defines ***/
#define CONTROL_SWITCH_VELOCITY (40)   //velocity to switch from open to close loop control


/*******************************************************************************************************************//**
 * @} (end defgroup TIMER_API)
 **********************************************************************************************************************/
#endif /* DRV_MOTOR_API_CHANGED_H */
