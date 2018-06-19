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
 * File Name    : sf_motion.c
 * Description  : Motion Control Framework.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include <string.h>
#include <framework/instances/sf_motion.h>
#include "hal_data.h"
#include "bsp_api.h"

#include "sf_motion_private_api.h"

extern int32_t     g_motor_count;
extern motor_instance_t const *g_motors[];

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void update_ctrl(sf_motion_ctrl_t *cm, sf_motion_stop_mode_t stop_request);
static void servo_on(sf_motion_ctrl_t * const p_ctrl);
static void servo_off(sf_motion_ctrl_t * const p_ctrl);
static void sf_halls(sf_motion_ctrl_t * const p_ctrl);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
/** Version data structure used by error logger macro. */
static const ssp_version_t g_sf_motion_version =
{
    .api_version_minor  = SF_MOTION_API_VERSION_MINOR,
    .api_version_major  = SF_MOTION_API_VERSION_MAJOR,
    .code_version_major = SF_MOTION_CODE_VERSION_MAJOR,
    .code_version_minor = SF_MOTION_CODE_VERSION_MINOR
};

/***********************************************************************************************************************
 * Implementation of Interface
 **********************************************************************************************************************/
const sf_motion_api_t g_sf_motion_on_sf_motion =
{
        .open          = SF_MOTION_Open,
        .close         = SF_MOTION_Close,
        .versionGet    = SF_MOTION_VersionGet,
        .start         = SF_MOTION_Start,
        .power         = SF_MOTION_Power,
        .capture       = SF_MOTION_Capture,
        .align         = SF_MOTION_Align,
        .servo         = SF_MOTION_Servo,
        .stop          = SF_MOTION_Stop,
        .logging       = SF_MOTION_Logging
};

static  int16_t g_instance_count = 0;
static  sf_motion_ctrl_t * gp_ctrl[MAX_SF_MOTION];

/*******************************************************************************************************************//**
 * @addtogroup SF_MOTION
 * @{
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Configures Motion Control framework .
 *
 * The SF_MOTION_Open function ....
 *
 * @retval SSP_SUCCESS           Initialization was successful.
 * @retval SSP_ERR_ASSERTION     One of the following parameters may be NULL: p_ctrl, p_api, or p_cfg.  See HAL driver
 *                               for other possible causes.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 *
 * @note This function is reentrant for any unit.
 *
 **********************************************************************************************************************/
ssp_err_t SF_MOTION_Open (sf_motion_ctrl_t * const p_ctrl, sf_motion_cfg_t * const p_cfg)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_cfg);
    SSP_ASSERT(NULL != p_cfg->p_encoder);
    SSP_ASSERT(NULL != p_cfg->p_motor);
    SSP_ASSERT(g_instance_count < MAX_SF_MOTION);
#endif

    p_ctrl->p_cfg = p_cfg;
    p_ctrl->act_state = 0;
    p_ctrl->hall_bit_u = 1;
    p_ctrl->hall_bit_v = 2;
    p_ctrl->hall_bit_w = 4;
    p_ctrl->halls_present = p_cfg->pin_hall_u && p_cfg->pin_hall_v && p_cfg->pin_hall_w;
    p_ctrl->vpg_state = VPG_MotionCompleted;
    sf_halls(p_ctrl);
    if (p_cfg->phasing_pars.mode == PIM_HALLS_BASED && p_ctrl->halls_present)
        p_ctrl->act_state |= ACT_Aligning;

    p_cfg->pos_loop_dflt.ds_time_ts = p_cfg->pos_loop_dflt.ds_time_us / 50; /* TODO: Replace the constant with the PWM period */
    if (p_cfg->pos_loop_dflt.ds_time_ts == 0)
        p_cfg->pos_loop_dflt.ds_time_ts = 1;
    if (p_cfg->pos_loop_dflt.out_limit == 0)
        p_cfg->pos_loop_dflt.out_limit = 32700;

    memcpy(&(p_ctrl->pos_loop_req), &(p_cfg->pos_loop_dflt), sizeof(p_cfg->pos_loop_dflt));


    /* Set default motion parameters */
    p_ctrl->motion_req.velocity = 2000;
    p_ctrl->motion_req.acceleration = 5;

    /* Open encoder interface */
    p_ctrl->p_encoder_api = p_cfg->p_encoder->p_api;
    p_ctrl->p_encoder_cfg = p_cfg->p_encoder->p_cfg;
    p_ctrl->p_encoder_ctrl = p_cfg->p_encoder->p_ctrl;
    p_cfg->p_encoder->p_api->open(p_cfg->p_encoder->p_ctrl, p_cfg->p_encoder->p_cfg);

    /* Open motor interface */
    p_ctrl->p_motor_api = p_cfg->p_motor->p_api;
    p_ctrl->p_motor_cfg = p_cfg->p_motor->p_cfg;
    p_ctrl->p_motor_ctrl = p_cfg->p_motor->p_ctrl;

    p_ctrl->auto_brake = p_ctrl->p_cfg->pin_brake ? true : false;

    g_motors[g_motor_count++] = p_cfg->p_motor;

    /* Make sure only the first instance sets callback function */
    p_ctrl->p_motor_cfg->callback = (g_instance_count == 0) ? SF_MOTION_Control : NULL;
    /* Make sure the number of instances does not exceed the limit */
    if (g_instance_count < MAX_SF_MOTION)
    {
        gp_ctrl[g_instance_count++] = p_ctrl;
        p_cfg->p_motor->p_api->open(p_ctrl->p_motor_ctrl, p_ctrl->p_motor_cfg);
    }
    return SSP_SUCCESS;
} /* End of function SF_MOTION_Open */

/*******************************************************************************************************************//**
 * @brief The close function releases any resources such as timers, ADCs and interrupts acquired by the Motion Framework.
 *
 * @retval  SSP_SUCCESS          Successful close.
 * @retval  SSP_ERR_ASSERTION    p_ctrl or p_ctrl->p_api is NULL.
 * @retval  SSP_ERR_NOT_OPEN     Driver control block not valid.  Call ::SF_MOTION_Open to configure.
 * @return                       See @ref Common_Error_Codes or HAL driver for other possible return codes or causes.
 **********************************************************************************************************************/
ssp_err_t SF_MOTION_Close (sf_motion_ctrl_t * const p_ctrl)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_encoder_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_motor_ctrl);
#endif

    /* Close motor interface */
    /* Close encoder interface */

    return SSP_SUCCESS;
} /* End of function SF_MOTION_Close */

/******************************************************************************************************************//**
 * @brief Gets version and stores it in provided pointer p_version.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_VersionGet (ssp_version_t * const p_version)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_sf_motion_version.version_id;

    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief Starts motion based on the currently set motion parameters and target position.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Start(sf_motion_ctrl_t * const p_ctrl)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif
    update_ctrl(p_ctrl, MOTION_STOP_NONE);
    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief Starts position capture operation
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Capture(sf_motion_ctrl_t * const p_ctrl, uint32_t code)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_encoder_api);
#endif
    p_ctrl->p_encoder_api->trigger(p_ctrl->p_encoder_ctrl, code);
    p_ctrl->act_state = (uint16_t)(p_ctrl->act_state & ~ACT_PosCaptured);
    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief Starts rotor phase alignment procedure
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Align(sf_motion_ctrl_t * const p_ctrl, sf_phasing_mode_t mode)
{
    /* Set flux vector angle */
    p_ctrl->p_motor_ctrl->phase_sector = 0;

    /* Set current phase init mode and activate phasing state */
    p_ctrl->p_cfg->phasing_pars.mode = mode;
    p_ctrl->phasing_timer = p_ctrl->p_cfg->phasing_pars.time;
    p_ctrl->act_state &= (uint16_t)~ACT_Aligned;
    p_ctrl->act_state |= ACT_Aligning;
    return SSP_SUCCESS;
}

/******************************************************************************************************************//**
 * @brief Stops motion based on the specified method.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Stop(sf_motion_ctrl_t * const p_ctrl, sf_motion_stop_mode_t stop_mode)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif
    update_ctrl(p_ctrl, stop_mode);
    return SSP_SUCCESS;
}

static bool sf_amp_fault(sf_motion_ctrl_t * const p_ctrl)
{
    ioport_level_t fault;
    g_ioport.p_api->pinRead(p_ctrl->p_cfg->pin_fault, &fault);
    return fault == 0;
}

/******************************************************************************************************************//**
 * @brief Controls power amplifier.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Power(sf_motion_ctrl_t * const p_ctrl, bool on)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif

    if (on)
    {
        /* Setup motor commutation based on the Encoder Counts Per Revolution */
        p_ctrl->p_motor_api->setup(p_ctrl->p_motor_ctrl, p_ctrl->p_encoder_cfg->ec_per_rev);

        /* Amplifier Enable */
        if (sf_amp_fault(p_ctrl))
            g_ioport.p_api->pinWrite(p_ctrl->p_cfg->pin_ae, false);

        p_ctrl->fault_timer = 0;
        g_ioport.p_api->pinWrite(p_ctrl->p_cfg->pin_ae, true);

        /* Status update */
        p_ctrl->act_state &= (uint16_t)~(ACT_AmpFault | ACT_PosError | ACT_WrapAround | ACT_OverCurrent | ACT_Inhibit);
        p_ctrl->act_state |= ACT_PowerOn;
    }
    else
    {
        /* Status update */
        p_ctrl->act_state &= (uint16_t)(~(ACT_PowerOn|ACT_ServoOn));
        /* Amplifier disable */
        g_ioport.p_api->pinWrite(p_ctrl->p_cfg->pin_ae, false);
        /* Servo Off */
        servo_off(p_ctrl);
    }
    return SSP_SUCCESS;
}

static void servo_on(sf_motion_ctrl_t * const p_ctrl)
{
    if ((p_ctrl->act_state & ACT_ServoOn) == 0)
    {
        p_ctrl->vpg_state = VPG_MotionCompleted;
        p_ctrl->motion_profile_points = 0;
        p_ctrl->motion_req.position = p_ctrl->position;
        p_ctrl->motion_cmd.position = p_ctrl->position;

        p_ctrl->p_motor_ctrl->foc_id_err_int = 0;
        p_ctrl->p_motor_ctrl->foc_iq_err_int = 0;
        p_ctrl->integral_err = 0;

        p_ctrl->p_motor_ctrl->adc1_offs = p_ctrl->p_motor_ctrl->adc1_raw;
        p_ctrl->p_motor_ctrl->adc2_offs = p_ctrl->p_motor_ctrl->adc2_raw;

        p_ctrl->act_state |= ACT_ServoOn | ACT_MotionComplete;
   }
    p_ctrl->act_state &= (uint16_t)~(ACT_AmpFault | ACT_PosError | ACT_WrapAround | ACT_OverCurrent | ACT_Inhibit);
}

static void servo_off(sf_motion_ctrl_t * const p_ctrl)
{
    p_ctrl->vpg_state = VPG_MotionCompleted;
    p_ctrl->motion_profile_points = 0;
    p_ctrl->position_err = 0;
    p_ctrl->integral_err = 0;
    p_ctrl->derivative_err = 0;
    //p_ctrl->pos_loop_cmd = p_ctrl->pos_loop_cmd.bias;
    p_ctrl->p_motor_ctrl->output_q = p_ctrl->pos_loop_cmd.bias;
    p_ctrl->p_motor_ctrl->output_d = 0;
    p_ctrl->act_state |= ACT_MotionComplete;
    p_ctrl->act_state &= (uint16_t)(~(ACT_ServoOn|ACT_Busy));
    if (p_ctrl->motion_req.profile_mode == VPGM_VelContouring)
        p_ctrl->motion_req.profile_mode = p_ctrl->p_cfg->profile_mode;
}


/******************************************************************************************************************//**
 * @brief Controls servo operation.
 *
 * @retval SSP_SUCCESS           Version returned successfully.
 * @retval SSP_ERR_ASSERTION     Parameter p_version was null.
 *********************************************************************************************************************/
ssp_err_t SF_MOTION_Servo(sf_motion_ctrl_t * const p_ctrl, bool on)
{
#if SF_MOTION_CFG_PARAM_CHECKING_ENABLE
    SSP_ASSERT(NULL != p_ctrl);
#endif

    /* Make sure the power is on and the motor is idle */
    if ((p_ctrl->act_state & ACT_PowerOn) == 0)
    {
        SF_MOTION_Power(p_ctrl, true);
    }
    if (on)
    {
        if (p_ctrl->p_motor_ctrl->commutation_mode != COMMUTATION_FORCED &&
            p_ctrl->p_motor_ctrl->commutation_mode != COMMUTATION_EXT)
        {
            servo_on(p_ctrl);
            update_ctrl(p_ctrl, MOTION_STOP_ABRUPT);
        }
    }
    else
    {
        servo_off(p_ctrl);
    }
    return SSP_SUCCESS;
}

static void sf_vpg(sf_motion_ctrl_t * const p_ctrl)
{
    /* Check if motion is started */
    if (p_ctrl->vpg_state == VPG_MotionCompleted)
        return;

    /* Is there a room in the FIFO? */
    if (p_ctrl->motion_profile_points >= MOTION_VPG_FIFO_SIZE)
        return;

   // Invoke profile generator depending on the profile mode
    switch (p_ctrl->motion_cmd.profile_mode)
    {
    case VPGM_Trapezoidal:
        vpg_trap_next (p_ctrl, p_ctrl->motion_profile_push_1, p_ctrl->motion_profile_push, false);
        break;
    case VPGM_VelContouring:
        vpg_trap_next (p_ctrl, p_ctrl->motion_profile_push_1, p_ctrl->motion_profile_push, true);
        break;
    case VPGM_Spline:
    case VPGM_PathPoints:
    case VPGM_PosVelTime:
    case VPGM_Bezier:
    default:
        return;
    }
    p_ctrl->motion_profile_push_1 = p_ctrl->motion_profile_push;
    p_ctrl->motion_profile_push++;
    if (p_ctrl->motion_profile_push == &p_ctrl->motion_profile_fifo[MOTION_VPG_FIFO_SIZE])
        p_ctrl->motion_profile_push = &p_ctrl->motion_profile_fifo[0];
    p_ctrl->motion_profile_points++;
}

static int16_t pid_calc(sf_motion_ctrl_t * const p_ctrl, int32_t pos_err)
{
    int32_t pid_out;

    /* Clip position error to signed short integer */
    if (pos_err > 32767)
      pos_err = 32767;
    else if (pos_err < -32767)
      pos_err = -32767;

    /* Calculate the position error derivative */
    p_ctrl->derivative_err = pos_err - p_ctrl->position_err;
    p_ctrl->position_err = pos_err;

    /* Integrate the position error */
    p_ctrl->integral_err += pos_err;

    /* Clip integral within the Integral Limit parameter */
    if (p_ctrl->integral_err > p_ctrl->pos_loop_cmd.il)
        p_ctrl->integral_err = p_ctrl->pos_loop_cmd.il;
    else if (p_ctrl->integral_err < -p_ctrl->pos_loop_cmd.il)
        p_ctrl->integral_err = -p_ctrl->pos_loop_cmd.il;

    /* Add proportional, derivative and integral terms */
    pid_out = (p_ctrl->integral_err * p_ctrl->pos_loop_cmd.ki) >> 8;

    pid_out += p_ctrl->derivative_err * p_ctrl->pos_loop_cmd.kd;

    pid_out += pos_err * p_ctrl->pos_loop_cmd.kp;

    /* Add velocity and acceleration feed forward */
    pid_out += (p_ctrl->pos_loop_cmd.kvff * (p_ctrl->motion_cmd.velocity >> 8)) >> 8;

    pid_out += (p_ctrl->pos_loop_cmd.kaff * (p_ctrl->motion_cmd.acceleration >> 8)) >> 8;

    /* Add output bias */
    pid_out += p_ctrl->pos_loop_cmd.bias;

    /* Clip PID regulator output within the Motor Limit parameter */
    if (pid_out > (int32_t)p_ctrl->pos_loop_cmd.out_limit)
        pid_out = p_ctrl->pos_loop_cmd.out_limit;
    else if (pid_out < -(int32_t)p_ctrl->pos_loop_cmd.out_limit)
        pid_out = -(int32_t)p_ctrl->pos_loop_cmd.out_limit;

    return (int16_t)pid_out;
}

static void sf_pos_loop(sf_motion_ctrl_t * const p_ctrl)
{
    /* Determine if position Control Loop time expired (in PWM Carrier time slices) */
    p_ctrl->pos_loop_timer--;
    if (p_ctrl->pos_loop_timer > 0)
        return;

    /* Reset position Loop Timer */
    p_ctrl->pos_loop_timer = p_ctrl->pos_loop_time;

    /* Measure the current velocity */
    p_ctrl->crnt_vel = p_ctrl->position - p_ctrl->position_1;
    p_ctrl->position_1 = p_ctrl->position;
    /* TODO: Add velocity observer or LP Filter for velocity measured */

    /* Implement velocity Profile Consumption */
    if (p_ctrl->vpg_state != VPG_MotionCompleted)
    {
        /* Check if new velocity / position setpoint is available */
        if (p_ctrl->motion_profile_points != 0)
        {
            p_ctrl->vpg_state = p_ctrl->motion_profile_pull->vpg_state;
            p_ctrl->motion_cmd.position = p_ctrl->motion_profile_pull->position;
            p_ctrl->motion_cmd.velocity = p_ctrl->motion_profile_pull->velocity;
            p_ctrl->motion_cmd.acceleration = p_ctrl->motion_profile_pull->acceleration;

            p_ctrl->motion_profile_pull++;
            if (p_ctrl->motion_profile_pull == &p_ctrl->motion_profile_fifo[MOTION_VPG_FIFO_SIZE])
                p_ctrl->motion_profile_pull = &p_ctrl->motion_profile_fifo[0];
            p_ctrl->motion_profile_points--;

            if (p_ctrl->vpg_state == VPG_MotionCompleted)
                p_ctrl->act_state |= ACT_MotionComplete;

        } else {
            /* Indicate VPG FIFO is depleted. */
            /* TODO: set an error flag, stop motion if Overrun exceeds a limit */
            p_ctrl->vpg_overrun++;
        }
    }

    /* Calculate PID regulator result */
    if (p_ctrl->act_state & ACT_ServoOn)
    {
        p_ctrl->p_motor_ctrl->output_q = pid_calc(p_ctrl, p_ctrl->motion_cmd.position - p_ctrl->p_encoder_ctrl->crnt.position);
    }
    else
    {
        p_ctrl->position_err = 0;
        p_ctrl->motion_cmd.position = p_ctrl->p_encoder_ctrl->crnt.position;
    }
}

static void sf_halls(sf_motion_ctrl_t * const p_ctrl)
{
    ioport_level_t hu, hv, hw;
    uint16_t    tmp = 0;

    /* Save old state for detecting transition */
    p_ctrl->hall_state_1 = p_ctrl->hall_state;

    /* Read the hall sensors */
    g_ioport.p_api->pinRead(p_ctrl->p_cfg->pin_hall_u, &hu);
    g_ioport.p_api->pinRead(p_ctrl->p_cfg->pin_hall_v, &hv);
    g_ioport.p_api->pinRead(p_ctrl->p_cfg->pin_hall_w, &hw);

    /* Map hall sensors to register bits */
    if (hu == IOPORT_LEVEL_HIGH)
        tmp |= p_ctrl->hall_bit_u;
    if (hv == IOPORT_LEVEL_HIGH)
        tmp |= p_ctrl->hall_bit_v;
    if (hw == IOPORT_LEVEL_HIGH)
        tmp |= p_ctrl->hall_bit_w;

    p_ctrl->hall_state = tmp;
    /* Check for hall sensors transition */
    if (p_ctrl->hall_state_1 != p_ctrl->hall_state)
    {
        p_ctrl->hall_change_pos = p_ctrl->position;
    }

    /* Communicate the hall state to the motor driver */
    p_ctrl->p_motor_ctrl->hall_state = p_ctrl->hall_state;
}

static void sf_interlocks(sf_motion_ctrl_t * const p_ctrl)
{
    /* Check for amplifier fault */
    if (sf_amp_fault(p_ctrl) &&
        (p_ctrl->act_state & ACT_AmpFault) == 0 /*&& p_ctrl->tc_limit != 0*/)
    {
        /* 500 us timeout for filtering noise on the fault input */
        if (++p_ctrl->fault_timer >= 5)
        {
            servo_off(p_ctrl);
            p_ctrl->act_state |= ACT_AmpFault;
            p_ctrl->act_state &= (uint16_t)(~ACT_PowerOn);
            p_ctrl->fault_timer = 0;
        }
    }
    else
    {
        p_ctrl->fault_timer = 0;
    }
    p_ctrl->act_state_1 = p_ctrl->act_state;
}

static void sf_phasing(sf_motion_ctrl_t * const p_ctrl)
{
    switch (p_ctrl->p_cfg->phasing_pars.mode)
    {
    case PIM_HALLS_BASED:
        if (p_ctrl->halls_present)
        {
            int32_t angle = 0;

            /* Check for hall sensors transition */
            if (p_ctrl->hall_state_1 == p_ctrl->hall_state || p_ctrl->hall_state == 0 || p_ctrl->hall_state == 7)
                return;

            /* TODO: Validate the transition by comparing the position to 1/6 ecpr value */

            /* Determine the angle of the rotor */
            switch (p_ctrl->hall_state_1)
            {
            case 2:     /* 0 - 60 deg sector */
                if (p_ctrl->hall_state == 6)
                    angle = 60;
                else if (p_ctrl->hall_state == 3)
                    angle = 0;
                else
                    return;
                break;
            case 6:     /* 60 - 120 deg sector */
                if (p_ctrl->hall_state == 4)
                    angle = 120;
                else if (p_ctrl->hall_state == 2)
                    angle = 60;
                else
                    return;
                break;
            case 4:     /* 120 - 180 deg sector */
                if (p_ctrl->hall_state == 5)
                    angle = 180;
                else if (p_ctrl->hall_state == 6)
                    angle = 120;
                else
                    return;
                break;
            case 5:     /* 180 - 240 deg sector */
                if (p_ctrl->hall_state == 1)
                    angle = 240;
                else if (p_ctrl->hall_state == 4)
                    angle = 180;
                else
                    return;
                break;
            case 1:     /* 240 - 300 deg sector */
                if (p_ctrl->hall_state == 3)
                    angle = 300;
                else if (p_ctrl->hall_state == 5)
                    angle = 240;
                else
                    return;
                break;
            case 3:     /* 300 - 360 deg sector */
                if (p_ctrl->hall_state == 2)
                    angle = 0;
                else if (p_ctrl->hall_state == 1)
                    angle = 300;
                else
                    return;
                break;
            default:
                return;
            }

            /* Set the desired flux orientation 90 deg away from the current position */
            p_ctrl->p_motor_ctrl->phase_origin = p_ctrl->position - (int32_t)(((double)(angle - 30) * p_ctrl->p_motor_ctrl->ec_per_ecycle) / 360.0);

            /* Indicate the phasing is completed */
            p_ctrl->act_state &= (uint16_t)~ACT_Aligning;
            p_ctrl->act_state |= ACT_Aligned;
        }
        break;

    case PIM_FORCED:
        if (p_ctrl->phasing_timer == 0)
            return;

        /* Check if this is the first entry into the wait state */
        if (p_ctrl->phasing_timer == p_ctrl->p_cfg->phasing_pars.time)
        {
            /* Apply Phase Voltage*/
            p_ctrl->p_motor_ctrl->output_q = p_ctrl->p_cfg->phasing_pars.power;
            p_ctrl->p_motor_ctrl->phase_sector = 0;
            /* Set FORCED commutation temporarily */
            p_ctrl->p_motor_ctrl->commutation_mode = COMMUTATION_FORCED;
        }

        /* Waits motor to settle for X time slices */
        p_ctrl->phasing_timer--;
        if (p_ctrl->phasing_timer > 0)
            return;

        /* Set the phase origin to the current position */
        p_ctrl->p_motor_ctrl->phase_origin = p_ctrl->position;

        /* Set the current position equal to the command position to avoid position error (in case servo is on) */
        p_ctrl->motion_cmd.position = p_ctrl->position;

        /* Indicate the phasing is completed */
        p_ctrl->act_state &= (uint16_t)~ACT_Aligning;
        p_ctrl->act_state |= ACT_Aligned;

        /* Remove Phase Voltage */
        p_ctrl->p_motor_ctrl->output_q = 0;

        /* Restore configured commutation mode */
        p_ctrl->p_motor_ctrl->commutation_mode = p_ctrl->p_motor_cfg->commutation_mode;
        break;

    default:
        break;
    }
}

static void recorder(sf_motion_ctrl_t *p_ctrl)
{
    if (p_ctrl->trace.mode == 1)
    {
        long clevel = (p_ctrl->trace.code[2] < 8) ? *(long *)p_ctrl->trace.p_data[2] : (long)(*p_ctrl->trace.p_data[2]);

        switch (p_ctrl->trace.trigger)
        {
        case 3:         /* Start on begin of motion, Stop on End of motion */
            if (p_ctrl->act_state & ACT_ServoOn)
            {
                if (!(p_ctrl->act_state & ACT_MotionComplete))
                    p_ctrl->trace.mode = 3;      /* Start now, Stop on end of motion */
            } else {
                if (p_ctrl->p_motor_ctrl->output_q != 0)
                    p_ctrl->trace.mode = 3;
            }
            break;
        case 7:         /* Start on begin of motion (when servo is on) or PWM output change (when servo is off) */
            if (p_ctrl->act_state & ACT_ServoOn)
            {
                if (!(p_ctrl->act_state & ACT_MotionComplete))
                    p_ctrl->trace.mode = 2;      /* Start now, Stop on buffer full */
            } else {
                if (p_ctrl->p_motor_ctrl->output_q != 0 || p_ctrl->p_motor_ctrl->output_d != 0)
                    p_ctrl->trace.mode = 2;
            }
            break;
        case 8:         /* Start on input change (threshold is input mask) */
            break;
        case 9:         /* Start on value2 > threshold, Stop on buffer full */
           // TODO: Save the old value, compare with the new
            if (clevel > p_ctrl->trace.level)
            {
                p_ctrl->trace.mode = 2;
            }
            break;
        case 10:        /* Start on value2 < threshold, Stop on buffer full */
            // TODO: Save the old value, compare with the new
            if (clevel < p_ctrl->trace.level)
            {
                p_ctrl->trace.mode = 2;
            }
            break;
        }
    }
    else if (p_ctrl->trace.mode > 1) {
        if ((short)--p_ctrl->trace.skip <= 0)
        {
            long clevel;
            unsigned short * dp = &p_ctrl->trace.data[p_ctrl->trace.index];
            p_ctrl->trace.skip = p_ctrl->trace.rateMult;

            *(dp) = *p_ctrl->trace.p_data[0];
            if (p_ctrl->trace.code[0] < 8)
            {
                *(dp + TRACE_BUFFER_SIZE)  = *(p_ctrl->trace.p_data[0] + 1);
                clevel = *(long *)p_ctrl->trace.p_data[0];
            }
            else
            {
                clevel = (long)(*p_ctrl->trace.p_data[0]);
            }
            *(dp + TRACE_BUFFER_SIZE*2) = *p_ctrl->trace.p_data[1];
            *(dp + TRACE_BUFFER_SIZE*3) = *p_ctrl->trace.p_data[2];

            if (p_ctrl->trace.count < TRACE_BUFFER_SIZE)
                p_ctrl->trace.count++;

            if (p_ctrl->trace.mode == 2)
            {   /* Trace until buffer is full */
                if (p_ctrl->trace.count >= TRACE_BUFFER_SIZE)
                   p_ctrl->trace.mode = 0;
            }
            else
            {
                /* Check for stop trigger */
                switch (p_ctrl->trace.trigger)
                {
                case 3:  /* Stop on Motion End */
                    if (p_ctrl->vpg_state == VPG_MotionCompleted)
                    {
                        p_ctrl->trace.mode = 0;
                    }
                    break;
                case 4:  /* Stop on Input change */
                    if (false)
                    {
                        p_ctrl->trace.mode = 0;
                    }
                    break;
                case 5:  /* Value > Threshold */
                    if (clevel > p_ctrl->trace.level)
                    {
                        p_ctrl->trace.mode = 0;
                    }
                    break;
                case 6:  /* Value < Threshold */
                    if (clevel < p_ctrl->trace.level)
                    {
                        p_ctrl->trace.mode = 0;
                    }
                    break;
                }
            }
            p_ctrl->trace.index++;
            if (p_ctrl->trace.index >= TRACE_BUFFER_SIZE)
                p_ctrl->trace.index = 0;
        }
    }
}


void SF_MOTION_Control(void)
{
    int16_t idx = 0;
    g_ioport.p_api->pinWrite(IOPORT_PORT_02_PIN_07, IOPORT_LEVEL_HIGH);
    while (idx < g_instance_count)
    {
        sf_motion_ctrl_t * const p_ctrl = gp_ctrl[idx];

        /* Process interlocks */
        sf_interlocks(p_ctrl);

        /* Update position feedback from the encoder driver */
        p_ctrl->p_encoder_api->read(p_ctrl->p_encoder_ctrl);
        p_ctrl->position = p_ctrl->p_encoder_ctrl->crnt.position;
        p_ctrl->p_motor_ctrl->position = p_ctrl->position;
        if (p_ctrl->p_encoder_ctrl->crnt.armed == false)
        {
            p_ctrl->act_state |= ACT_PosCaptured;
        }

        /* Read hall sensor feedback */
        if (p_ctrl->halls_present)
            sf_halls(p_ctrl);

        /* Run Velocity Profile Generator */
        sf_vpg(p_ctrl);

        /* Run position loop control */
        sf_pos_loop(p_ctrl);

        /* Run phasing algorithm */
        if (p_ctrl->act_state & ACT_Aligning)
            sf_phasing(p_ctrl);

        /* Set new torque / speed set-point for the motor driver */
        p_ctrl->p_motor_api->control(p_ctrl->p_motor_ctrl);

        /* Record data values to trace buffers */
        recorder(p_ctrl);

        idx++;
    }
    g_ioport.p_api->pinWrite(IOPORT_PORT_02_PIN_07, IOPORT_LEVEL_LOW);
}

static void update_ctrl(sf_motion_ctrl_t *p_ctrl, sf_motion_stop_mode_t stop_request)
{
    /* If a relative target is requested - transform into absolute target */
    if (p_ctrl->position_rel_req != 0)
    {
        p_ctrl->motion_req.position = p_ctrl->position_rel_req + p_ctrl->p_encoder_ctrl->crnt.position;
        p_ctrl->position_rel_req = 0;
    }
    /* Update the motion profile parameters with the buffered one */
    memcpy(&(p_ctrl->motion_trgt), &(p_ctrl->motion_req), sizeof(p_ctrl->motion_req));

    /* If the deceleration is not explicitly defined use the acceleration instead */
    if (p_ctrl->motion_trgt.deceleration == 0)
        p_ctrl->motion_trgt.deceleration = p_ctrl->motion_trgt.acceleration;

    if (p_ctrl->pos_loop_req.il > 0x007fffff)
        p_ctrl->pos_loop_req.il = 0x007fffff;

    /* Disable interrupts that may affect the parameter changes */
    //__disable_interrupt();

    /* Update the PID parameters with the buffered one */
    memcpy(&(p_ctrl->pos_loop_cmd), &(p_ctrl->pos_loop_req), sizeof(p_ctrl->pos_loop_req));

    /* Setup position loop time slice as a multiple of PWM cycles */
    p_ctrl->pos_loop_time = 2;
    if (p_ctrl->pos_loop_cmd.ds_time_ts)
        p_ctrl->pos_loop_time = p_ctrl->pos_loop_cmd.ds_time_ts;

    /* Process stop_request */
    if (stop_request == MOTION_STOP_NONE)
    {
        if (p_ctrl->vpg_state == VPG_MotionCompleted)
        {
            p_ctrl->motion_cmd.profile_mode = p_ctrl->motion_req.profile_mode;
            switch (p_ctrl->motion_cmd.profile_mode) {
            case VPGM_Trapezoidal:
                p_ctrl->vpg_state = vpg_trap_start(p_ctrl, 0);      /* Start motion request in Trapezoidal Profile */
                break;
            case VPGM_VelContouring:
                p_ctrl->vpg_state = vpg_trap_start(p_ctrl, 1);      /* Start motion request in velocity Mode */
                break;
            default:
                break;
            }
            if (p_ctrl->vpg_state != VPG_MotionCompleted)
            {
                p_ctrl->act_state &= (uint16_t)~ACT_MotionComplete;
            }
            else
            {
                p_ctrl->act_state |= (uint16_t)ACT_MotionComplete;
            }
        }
        else
        {
            /* Change velocity profile on the fly only allowed for velocity mode */
            if (p_ctrl->motion_req.profile_mode == VPGM_VelContouring ||
                p_ctrl->motion_req.profile_mode == VPGM_Trapezoidal)
            {
                p_ctrl->motion_cmd.profile_mode = p_ctrl->motion_req.profile_mode;
                p_ctrl->direction = p_ctrl->motion_cmd.dir_sign;
                p_ctrl->motion_profile_points = 1;
                if (p_ctrl->motion_profile_pull->vpg_state == VPG_MotionCompleted)
                    p_ctrl->motion_profile_pull->vpg_state = VPG_TrapDeceleration;
            }
        }
    }
    else
    {
        /* If the motion is completed - ignore the stop request */
        if (p_ctrl->vpg_state == VPG_MotionCompleted)
        {
            p_ctrl->motion_trgt.position = p_ctrl->p_encoder_ctrl->crnt.position;
        }
        else
        {
            int32_t stop_dist = p_ctrl->motion_profile_pull->stop_dist;
            if (p_ctrl->motion_trgt.profile_mode != VPGM_Trapezoidal)
            {
                p_ctrl->motion_trgt.profile_mode = VPGM_Trapezoidal;
                p_ctrl->motion_req.profile_mode = VPGM_Trapezoidal;
                stop_dist = (int32_t)(((float)p_ctrl->motion_profile_pull->velocity * (float)p_ctrl->motion_profile_pull->velocity) / (float)(p_ctrl->motion_trgt.deceleration<<1)) >> 16;
            }
            if (stop_request == MOTION_STOP_ABRUPT)         /* Abrupt stop request */
            {
                stop_dist >>= 5;
                p_ctrl->motion_trgt.deceleration <<= 5;
            }
            /* Check the direction and adjust the target with the stopping distance */
            if (p_ctrl->motion_trgt.position > p_ctrl->p_encoder_ctrl->crnt.position)
                p_ctrl->motion_trgt.position = p_ctrl->p_encoder_ctrl->crnt.position + stop_dist;
            else
                p_ctrl->motion_trgt.position = p_ctrl->p_encoder_ctrl->crnt.position - stop_dist;
        }
        p_ctrl->motion_req.position = p_ctrl->motion_trgt.position;
    }

   //__enable_interrupt();
}

extern int16_t g_pwm_timer_count;

static void * decode_ptr (sf_motion_ctrl_t *p_ctrl, int32_t code);
static void * decode_ptr (sf_motion_ctrl_t *p_ctrl, int32_t code)
{
   switch (code) {
   case 0:  // Current position
      return (void *)&(p_ctrl->position);
   case 1:  // Commanded velocity
      return &(p_ctrl->motion_cmd.velocity);
   case 2:  // Commanded acceleration
      return &(p_ctrl->motion_cmd.acceleration);
   case 3:
    return 0; // &(p_ctrl->I2t_acc);
   case 8:  // Error
      return &(p_ctrl->position_err);
   case 9:  // PID Output
      return &(p_ctrl->p_motor_ctrl->output_q);
   case 10: // Total Current
      return &(p_ctrl->p_motor_ctrl->total_current);
   case 11: // Direct Current
      return &(p_ctrl->p_motor_ctrl->foc_id);
   case 12: // Quadrature Current
      return &(p_ctrl->p_motor_ctrl->foc_iq);
   case 13: // Direct Current Error
      return &(p_ctrl->p_motor_ctrl->foc_id_err);
   case 14: // Quadrature Current Error
      return &(p_ctrl->p_motor_ctrl->foc_iq_err);
   case 15: // Current A
      return &(p_ctrl->p_motor_ctrl->adc1_raw);
   case 16: // Current B
      return &(p_ctrl->p_motor_ctrl->adc2_raw);
   case 17: // Current Clamping
      return 0; //&(p_ctrl->p_motor_ctrl->clamp_mode);
   case 18: // FOC Voltage Output - D
      return &(p_ctrl->p_motor_ctrl->foc_vd);
   case 19: // FOC Voltage Output - Q
      return &(p_ctrl->p_motor_ctrl->foc_vq);
   case 20: // RealTime task timing
    return &g_pwm_timer_count;
   case 21: // Phase Angle
      return &(p_ctrl->p_motor_ctrl->phase_angle);
   }
   return (void *)&(p_ctrl->position_err);
}

ssp_err_t SF_MOTION_Logging (sf_motion_ctrl_t *p_ctrl, int32_t code, int32_t ch)
{
    if (ch >=0 && ch <= 3)
    {
        p_ctrl->trace.p_data[ch] = decode_ptr (p_ctrl, code);
        p_ctrl->trace.code[ch] = (int16_t)code;
    }
    return SSP_SUCCESS;
}


/*******************************************************************************************************************//**
 * @} (end addtogroup SF_MOTION)
 **********************************************************************************************************************/

