/***********************************************************************************************************************
 * Copyright [2016] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

/**********************************************************************************************************************
 * File Name    : r_motor_api.h
 * Description  : General timer API.  Allows for periodic interrupt creation with a callback function.
 **********************************************************************************************************************/

#ifndef DRV_MOTOR_API_H
#define DRV_MOTOR_API_H

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
#include "bsp_api.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Leading zeroes removed to avoid coding standard violation. */
#define MOTOR_API_VERSION_MAJOR ((uint8_t)1)
#define MOTOR_API_VERSION_MINOR ((uint8_t)0)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Motor types */
typedef enum e_motor_types
{
    MOTOR_STEPPER,      ///< Two-phase Stepper Motor
    MOTOR_ACIM,         ///< Three-phase AC Induction Motor
    MOTOR_DC,           ///< Single Phase Brushled DC Motor
    MOTOR_BLDC,         ///< Three-phase Brushless DC Motor
} motor_types_t;

/** Possible status values returned by motor_api_t::infoGet. */
typedef enum e_motor_status
{
    MOTOR_STATUS_DISABLED,   	///< Motor is idle, operation is disabled
    MOTOR_STATUS_ENABLED,   	///< Motor is enabled
    MOTOR_STATUS_FAULT,   		///< Motor fault (overcurrent, overvoltage, overtemperature, hall error)
} motor_status_t;

/** Motor commutation modes */
typedef enum e_motor_comm_mode
{
    COMMUTATION_SIN,			///< Sinusoidal Commutation with Space Vector Modulation
    COMMUTATION_FOC,  			///< Field Oriented Control Algorithm w/ SVM
    COMMUTATION_TRAP,			///< Trapezoidal / Scalar / 6-step Commmutaion mode
    COMMUTATION_FORCED,			///< Forced Commutation - Flux angle defined explicitly
    COMMUTATION_EXT 			///< External Commutation - Phase voltages defined explicitly
} motor_comm_mode_t;

/** Motor control modes */
typedef enum e_motor_ctrl_mode
{
    CONTROL_MODE_VOLTAGE,		///< Open loop - voltage vector control
    CONTROL_MODE_TORQUE,		///< Closed loop torque control
    CONTROL_MODE_SPEED			///< Closed loop speed control
} motor_ctrl_mode_t;


/** Bridge control method */
typedef enum e_pwm_out_mode
{
    PWM_OUT_INDEPENDENT_A,      ///< Independent control of the inverter - 3 PWM Outputs, Channel A
    PWM_OUT_INDEPENDENT_B,      ///< Independent control of the inverter - 3 PWM Outputs, Channel B
    PWM_OUT_COMPLEMENTARY 		///< Complementary control of the inverter upper / lower leg - 6 PWM Outputs
} pwm_out_mode_t;

/** Motor information structure to store various information for a motor resource */
typedef struct st_motor_info
{
    motor_status_t      status;
    uint8_t  			halls;			///< Hall-sensor status
    float	 			flux_angle;   	///< Rotor angle
    float	 			sin_flux_angle;	///< SIN (Rotor angle)
    float	 			cos_flux_angle;	///< SIN (Rotor angle)

    uint32_t 			current_q;   	///< Quadrature current
    uint32_t 			current_d;   	///< Direct current
    uint32_t 			current_q_err; 	///< Quadrature current error
    uint32_t 			current_d_err; 	///< Direct current error
    uint32_t 			integral_q;   	///< Quadrature error integral
    uint32_t 			integral_d;   	///< Direct error integral

	uint16_t			pwm_output_u;   ///< PWM Duty cycle of channel U
	uint16_t			pwm_output_v;   ///< PWM Duty cycle of channel V
	uint16_t			pwm_output_w;   ///< PWM Duty cycle of channel W

} motor_info_t;

/** User configuration structure, used in open function */
typedef struct st_motor_cfg
{
	/* Motor parameters */
    motor_types_t 		type;   		///< Motor type. Select enumerated value from ::motor_types_t
    uint8_t  			pole_pairs;   	///< Number of rotor pole pairs
    float               inductance_uH;  ///< Motor winding inductance [uH]
    float               bemf_v_krpm;    ///< Motor Back EMF Coefficient - V/kprm
    uint16_t            bus_voltage;    ///< Inverter bus voltage
    uint16_t            phase_mapping;  ///< Motor to Inverter phase mapping code

	/* Control algorithm configuration */
    motor_comm_mode_t   commutation_mode;    ///< Commutation mode. Select enumerated value from ::motor_comm_mode_t
    motor_ctrl_mode_t   control_mode;        ///< Control mode. Select enumerated value from ::motor_ctrl_mode_t

	/* PWM Generator configuration */
    uint8_t  			pwm_ch_u;		///< Phase U timer channel number.
    uint8_t  			pwm_ch_v; 		///< Phase V timer channel number.
    uint8_t  			pwm_ch_w; 		///< Phase W timer channel number.

    float     			dead_time_us;	///< Dead time setting [us]
    uint32_t            pwm_freq_KHz;	///< PWM Carrier Frequency (default 20KHz)
    pwm_out_mode_t      output_mode;    ///< Select enumerated value from ::pwm_out_mode_t

	/* Current feedback configuration */
    uint8_t  			adc_ch_u;		///< U Phase ADC channel number.
    uint8_t  			adc_ch_v;		///< V Phase ADC channel number.
    uint32_t            adc_range_mA;	///< ADC range in milliampers

    uint16_t  			crnt_limit_mA;      ///< Current limit
    uint16_t  			crnt_limit_time_ms; ///< Current limit time [us]

	/* FOC algorithm configuration */
    uint16_t  			kp_d, ki_d;		///< KP / KI - direct current loop
    uint16_t  			kp_q, ki_q;		///< KP / KI - quadrature current loop
    int32_t             feedback_gain;  ///< Feedback gain

    void        (*callback)(void);      ///< Callback pointer to a function to be invoked every PWM cycle
    void const  * p_extend;    			///< Extension parameter for hardware specific settings.
} motor_cfg_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when motor_api_t::open is called. */
typedef struct st_motor_ctrl
{
    motor_comm_mode_t   commutation_mode;    ///< Commutation mode. Select enumerated value from ::motor_comm_mode_t
    int32_t             position;
    uint16_t            hall_state;

    uint32_t            pwm_period;     ///< Number of timer counts per PWM full cycle
    uint32_t            pwm_period_2;   ///< Number of timer counts per PWM half cycle

    int32_t             ec_per_ecycle;  ///< Number of encoder counts per electrical cycle
    int32_t             ec_per_rev2;    ///< Number of encoder counts per revolution / 2
    int32_t             ec_per_rev4;    ///< Number of encoder counts per revolution / 4
    int32_t             ec_per_rev6;    ///< Number of encoder counts per revolution / 6

    /* Phasing / Alignment related parameters */
    int32_t             phase_origin;
    int32_t             phase_offset;
    int32_t             phase_angle;
    int32_t             phase_adv_gain;
    int32_t             phase_vel_comp;
    int32_t             phase_sector;
    motor_cfg_t         *p_cfg;

    /* Output variables */
    int16_t             output_q;           /* Position Loop Output - Current/Torque Setpoint */
    int16_t             output_d;
    float               out_u;              /* Phase voltages */
    float               out_v;
    float               out_w;

    /* Field Oriented Control Gains */
    int16_t             foc_kp, foc_ki;
    int32_t             foc_iq_err_int, foc_id_err_int;
    int16_t             foc_iq_err, foc_id_err;

    int16_t             foc_id, foc_iq;         /* Measured D and Q currents */
    int16_t             foc_vd, foc_vq;         /* Calculated D and Q voltages */
    float               foc_alpha, foc_beta;
    float               calc_alpha, calc_beta;
    float               counts2rad;             /* Converts angle from counts to radian */
    float               angle_rad;
    float               angle_sin;
    float               angle_cos;

    /* Raw ADC readings / Current Feedback */
    int16_t             adc1_raw, adc2_raw;
    int16_t             adc1_offs, adc2_offs;
    int16_t             adc_iu, adc_iv, adc_iw;
    int16_t             *p_iu, *p_iv;
    int16_t             total_current;

    R_GPTA0_Type        *p_gpt_u;
    R_GPTA0_Type        *p_gpt_v;
    R_GPTA0_Type        *p_gpt_w;

    IRQn_Type           irq;          ///< Counter overflow IRQ number
    uint32_t            open;         ///< Whether or not channel is open
} motor_ctrl_t;


/** Timer API structure. General timer functions implemented at the HAL layer follow this API. */
typedef struct st_motor_api
{
    /** Initial configuration.
     * @par Implemented as
     * - R_MOTOR_MotorOpen()
     *
     * @pre Peripheral clocks and any required output pins should be configured prior to calling this function.
     * @note To reconfigure after calling this function, call motor_api_t::close first.
     * @param[in]   p_ctrl     Pointer to control block. Must be declared by user. Elements set here.
     * @param[in]   p_cfg      Pointer to configuration structure. All elements of this structure must be set by user.
     */
    ssp_err_t (* open)(motor_ctrl_t      * p_ctrl,
                       motor_cfg_t       * const p_cfg);

    /** Setup motor commutation - maps U/V/W phases, calculates parameters based on encoder counts per revolution, sets ADC offset.
     * @par Implemented as
     * - R_MOTOR_Setup()
     *
     * @param[in]   p_ctrl     Control block set in motor_api_t::open call for this timer.
     */
    ssp_err_t (* setup)(motor_ctrl_t      * const p_ctrl, int32_t ecpr);

    /** Execute motor control algorithm.
     * @par Implemented as
     * - R_MOTOR_Control()
     *
     * @param[in]   p_ctrl     Control block set in motor_api_t::open call for this timer.
     */
    ssp_err_t (* control)(motor_ctrl_t   * const p_ctrl);

    /** Get the time until the timer expires in clock counts and store it in provided pointer p_period_counts.
     * @par Implemented as
     * - R_MOTOR_InfoGet()
     *
     * @param[in]   p_ctrl     Control block set in motor_api_t::open call for this motor.
     * @param[out]  p_info     Collection of information for this motor.
     */
    ssp_err_t (* infoGet)(motor_ctrl_t    * const p_ctrl,
                          motor_info_t    * const p_info);

    /** Allows driver to be reconfigured and may reduce power consumption.
     * @par Implemented as
     * - R_MOTOR_Close()
     *
     * @param[in]   p_ctrl     Control block set in motor_api_t::open call for this timer.
     */
    ssp_err_t (* close)(motor_ctrl_t      * const p_ctrl);

    /** Get version and store it in provided pointer p_version.
     * @par Implemented as
     * - R_MOTOR_VersionGet()
     *
     * @param[out]  p_version  Code and API version used.
     */
    ssp_err_t (* versionGet)(ssp_version_t     * const p_version);
} motor_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_motor_instance
{
    motor_ctrl_t       * const p_ctrl;    ///< Pointer to the control structure for this instance
    motor_cfg_t        * const p_cfg;     ///< Pointer to the configuration structure for this instance
    const motor_api_t        * p_api;     ///< Pointer to the API structure for this instance
} motor_instance_t;


/*******************************************************************************************************************//**
 * @} (end defgroup TIMER_API)
 **********************************************************************************************************************/
#endif /* DRV_MOTOR_API_H */
