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

/***********************************************************************************************************************
 * File Name    : r_motor_bldc.c
 * Description  : BLDC Motor control functions used to implement the Motor interfaces.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <math.h>
#include "r_motor_api.h"
#include "r_motor_bldc.h"
#include "r_motor_bldc_cfg.h"
#include "../src/driver/r_gpt/hw/hw_gpt_private.h"
#include "../src/driver/r_motor_bldc/r_motor_bldc_private_api.h"
#include "../src/driver/r_motor_bldc/hw/hw_motor_bldc_private.h"

#include "r_cgc.h"

void pwm_counter_overflow (void);

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define PI_LIMIT (100 * 256)

/** Macro for error logger. */
#ifndef GPT_ERROR_RETURN
#define GPT_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), "motor_bldc", &g_motor_bldc_version)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

#if defined(__GNUC__)
/* This structure is affected by warnings from the GCC compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60784
 * This pragma suppresses the warnings in this structure only, and will be removed when the SSP compiler is updated to
 * v5.3.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
/** Version data structure used by error logger macro. */
static const ssp_version_t g_motor_bldc_version =
{
    .api_version_minor  = MOTOR_API_VERSION_MINOR,
    .api_version_major  = MOTOR_API_VERSION_MAJOR,
    .code_version_major = MOTOR_BLDC_CODE_VERSION_MAJOR,
    .code_version_minor = MOTOR_BLDC_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/** BLDC Motor Driver Implementation */
const motor_api_t g_motor_bldc =
{
    .open            = R_Motor_BLDC_Open,
    .setup           = R_Motor_BLDC_Setup,
    .control         = R_Motor_BLDC_Control,
    .infoGet         = R_Motor_BLDC_InfoGet,
    .close           = R_Motor_BLDC_Close,
    .versionGet      = R_Motor_BLDC_VersionGet
};

extern void (*sf_callback)(void);
extern bool irq_handler_set;

static void motor_bldc_init(R_GPTA0_Type *p_gpt, uint16_t ch, uint32_t period, bool center_aligned, bool half_cycle_reload)
{
    HW_MTR_RegisterInit(p_gpt, ch, period);
    p_gpt->GTBER_b.CCRA = 1;   /* Enable single buffer operation */
    p_gpt->GTBER_b.CCRB = 1;

    p_gpt->GTBER_b.ADTTA = 1;
    p_gpt->GTBER_b.ADTTB = 1;

    if (center_aligned)
    {
        HW_GPT_ModeSet(p_gpt, half_cycle_reload ? GPT_MODE_TW_PWM_2 : GPT_MODE_TW_PWM_1);
    }
    else
    {
        HW_GPT_ModeSet(p_gpt, GPT_MODE_PERIODIC);
    }

}

volatile int32_t tu, tv, tw;

static void update_pwm(motor_ctrl_t * p_ctrl)
{
    /* Convert output (+0.5 .. -0.5) to 0 .. Period Count */
    tu = (int32_t)((float)p_ctrl->pwm_period * p_ctrl->out_u) + (int32_t)p_ctrl->pwm_period_2;
    tv = (int32_t)((float)p_ctrl->pwm_period * p_ctrl->out_v) + (int32_t)p_ctrl->pwm_period_2;
    tw = (int32_t)((float)p_ctrl->pwm_period * p_ctrl->out_w) + (int32_t)p_ctrl->pwm_period_2;

    /* Clip counter value within 1 .. Period Count-1 */
    if (tu >= (int32_t)p_ctrl->pwm_period)
        tu = (int32_t)p_ctrl->pwm_period - 1;
    if (tv >= (int32_t)p_ctrl->pwm_period)
        tv = (int32_t)p_ctrl->pwm_period - 1;
    if (tw >= (int32_t)p_ctrl->pwm_period)
        tw = (int32_t)p_ctrl->pwm_period - 1;

    if (tu <= 0)
        tu = 1;
    if (tv <= 0)
        tv = 1;
    if (tw <= 0)
        tw = 1;

    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_A)
    {
        p_ctrl->p_gpt_u->GTCCRC = (uint32_t)tu;
        p_ctrl->p_gpt_v->GTCCRC = (uint32_t)tv;
        p_ctrl->p_gpt_w->GTCCRC = (uint32_t)tw;
    }
    else
    {
        p_ctrl->p_gpt_u->GTCCRE = (uint32_t)tu;
        p_ctrl->p_gpt_v->GTCCRE = (uint32_t)tv;
        p_ctrl->p_gpt_w->GTCCRE = (uint32_t)tw;
    }
}

/* Space Vector Modulation */
static void commutate_svm(motor_ctrl_t *p_ctrl, float alpha, float beta)
{
    float _x, _y, _z;
    float _u, _v, _w;

    _x = beta;
    _y = (beta + alpha * MTR_SQRT_3) * 0.5f;
    _z = (beta - alpha * MTR_SQRT_3) * 0.5f;

    /* Determine the sector */
    if (_y < 0.0)
    {
        if (_z < 0.0)
        {
            /* Sector V */
            _u = (1 + _y - _z) * 0.5f;
            _v = _u + _z;
            _w = _u - _y;
        }
        else if (_x <= 0.0)
        {
            /* Sector IV */
            _u = (1 + _x - _z) * 0.5f;
            _v = _u + _z;
            _w = _v - _x;
        }
        else
        {
            /* Sector III */
            _u = (1 - _x + _y) * 0.5f;
            _w = _u - _y;
            _v = _w + _x;
        }
    }
    else
    {
        if (_z >= 0.0)
        {
            /* Sector II */
            _u = (1 + _y - _z) * 0.5f;
            _v = _u + _z;
            _w = _u - _y;
        }
        else if (_x <= 0.0)
        {
            /* Sector VI */
            _u = (1 - _x + _y) * 0.5f;
            _w = _u - _y;
            _v = _w + _x;
        }
        else
        {
            /* Sector I */
            _u = (1 + _x - _z) * 0.5f;
            _v = _u + _z;
            _w = _v - _x;
        }
    }

    /* Shift and Scale the result range from 0 .. 1 to -0.5 .. 0.5 */
    p_ctrl->out_u = (_u - 0.5f);
    p_ctrl->out_v = (_v - 0.5f);
    p_ctrl->out_w = (_w - 0.5f);
}

/* Field-Oriented Control + Space Vector Modulation */
static void commutate_foc(motor_ctrl_t *p_ctrl)
{
    float alpha, beta;
    int32_t vd, vq;

    // Clark Transformation: Calculate alpha and beta angles from the measured current
    alpha = (float)(*p_ctrl->p_iu);
    beta = (float)(*p_ctrl->p_iu + *p_ctrl->p_iv + *p_ctrl->p_iv) * MTR_1_SQRT_3;    // 1/SQUAREROOT(3)

    /* Scale the feedback based on the 12-bit ADC reading (range is +/-2048) */
    /* Scale the feedback in the range of +/-32767 */
    alpha *= 32767.0f/(3 * 2048 * MTR_SQRT_3);
    beta  *= 32767.0f/(3 * 2048 * MTR_SQRT_3);

    // Park Transformation: Calculate ID and IQ
    p_ctrl->foc_id = (int16_t)(alpha*p_ctrl->angle_cos + beta*p_ctrl->angle_sin);
    p_ctrl->foc_iq = (int16_t)(beta*p_ctrl->angle_cos - alpha*p_ctrl->angle_sin);

    // Calculate PI current control loops
    p_ctrl->foc_id_err = (int16_t)((p_ctrl->output_d >> 2) - p_ctrl->foc_id);
    p_ctrl->foc_iq_err = (int16_t)((p_ctrl->output_q >> 2) - p_ctrl->foc_iq);

    p_ctrl->foc_id_err_int += (int32_t)(p_ctrl->foc_id_err);
    p_ctrl->foc_iq_err_int += (int32_t)(p_ctrl->foc_iq_err);

    /* Limit IQ and ID integral error */
    if (p_ctrl->foc_id_err_int > PI_LIMIT)
        p_ctrl->foc_id_err_int = PI_LIMIT;
    else if (p_ctrl->foc_id_err_int < -PI_LIMIT)
        p_ctrl->foc_id_err_int = -PI_LIMIT;
    if (p_ctrl->foc_iq_err_int > PI_LIMIT)
        p_ctrl->foc_iq_err_int = PI_LIMIT;
    else if (p_ctrl->foc_iq_err_int < -PI_LIMIT)
        p_ctrl->foc_iq_err_int = -PI_LIMIT;

    /* Calculate PI regulator output - Q and D values */
    vd = (((long)p_ctrl->foc_id_err * p_ctrl->foc_kp) >> 4) + ((p_ctrl->foc_id_err_int * p_ctrl->foc_ki) >> 12);
    vq = (((long)p_ctrl->foc_iq_err * p_ctrl->foc_kp) >> 4) + ((p_ctrl->foc_iq_err_int * p_ctrl->foc_ki) >> 12);

    /* Limit output */
    if (vd > 32767)
        vd = 32767;
    else if (vd < -32767)
        vd = -32767;
    if (vq > 32767)
        vq = 32767;
    else if (vq < -32767)
        vq = -32767;

    p_ctrl->foc_vq = (int16_t)vq;
    p_ctrl->foc_vd = (int16_t)vd;

    // Inverse Park Transformation
    p_ctrl->foc_alpha = p_ctrl->foc_vd * p_ctrl->angle_cos - p_ctrl->foc_vq * p_ctrl->angle_sin;
    p_ctrl->foc_beta  = p_ctrl->foc_vq * p_ctrl->angle_cos + p_ctrl->foc_vd * p_ctrl->angle_sin;
    // TODO: Scale down alpha and beta if they exceed the max radius vector

}

static float vect_u[] = { MTR_2_3,  MTR_1_3, -MTR_1_3, -MTR_2_3, -MTR_1_3,  MTR_1_3};
static float vect_v[] = {-MTR_1_3,  MTR_1_3,  MTR_2_3,  MTR_1_3, -MTR_1_3, -MTR_2_3};
static float vect_w[] = {-MTR_1_3, -MTR_2_3, -MTR_1_3,  MTR_1_3,  MTR_2_3,  MTR_1_3};

/* Trapezoidal commutation using one of six possible sectors */
static void commutate_set(motor_ctrl_t *p_ctrl)
{

    if (p_ctrl->phase_sector >= 0 && p_ctrl->phase_sector <= 5)
    {
        p_ctrl->out_u = vect_u[p_ctrl->phase_sector] * 0.5f * (float)p_ctrl->output_q * (1/32768.0f);
        p_ctrl->out_v = vect_v[p_ctrl->phase_sector] * 0.5f * (float)p_ctrl->output_q * (1/32768.0f);
        p_ctrl->out_w = vect_w[p_ctrl->phase_sector] * 0.5f * (float)p_ctrl->output_q * (1/32768.0f);
    }
    else
    {
        p_ctrl->out_u = 0;
        p_ctrl->out_v = 0;
        p_ctrl->out_w = 0;
    }
}

/* Hall sensor combinations

    Sector  Binary  Decimal
    0  - 60 010     2
    60 -120 110     6
    120-180 100     4
    180-240 101     5
    240-300 001     1
    300-360 011     3
*/

static int32_t sector[] = {
    0 * 60,      /* 0 */
    4 * 60,      /* 1 */
    0 * 60,      /* 2 */
    5 * 60,      /* 3 */
    2 * 60,      /* 4 */
    3 * 60,      /* 5 */
    1 * 60,      /* 6 */
    0 * 60,      /* 7 */
};

/*******************************************************************************************************************//**
 * @addtogroup Motor
 * @{
 **********************************************************************************************************************/


static void map_channels(motor_ctrl_t * const p_ctrl, uint8_t *p_ch_u, uint8_t *p_ch_v, uint8_t *p_ch_w)
{
    switch (p_ctrl->p_cfg->phase_mapping)
    {
    case 0:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_u;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_v;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_w;
        //pm->p_iu = &(pm->adc_iu);
        //pm->p_iv = &(pm->adc_iv);
        break;
    case 1:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_u;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_w;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_v;
        //pm->p_iu = &(pm->adc_iu);
        //pm->p_iv = &(pm->adc_iw);
        break;
    case 2:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_v;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_w;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_u;
        //pm->p_iu = &(pm->adc_iv);
        //pm->p_iv = &(pm->adc_iw);
        break;
    case 3:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_v;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_u;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_w;
        //pm->p_iu = &(pm->adc_iv);
        //pm->p_iv = &(pm->adc_iu);
        break;
    case 4:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_w;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_u;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_v;
        //pm->p_iu = &(pm->adc_iw);
        //pm->p_iv = &(pm->adc_iu);
        break;
    case 5:
    default:
        *p_ch_u = p_ctrl->p_cfg->pwm_ch_w;
        *p_ch_v = p_ctrl->p_cfg->pwm_ch_v;
        *p_ch_w = p_ctrl->p_cfg->pwm_ch_u;
        //pm->p_iu = &(pm->adc_iw);
        //pm->p_iv = &(pm->adc_iv);
        break;
    }
}

static ssp_err_t gpt_common_open (motor_ctrl_t * const p_ctrl)
{
    uint8_t ch_u, ch_v, ch_w;
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0}};
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_GPT;

    fmi_feature_info_t info = {0U};
    map_channels(p_ctrl, &ch_u, &ch_v, &ch_w);

    ssp_err_t err;
    ssp_feature.channel = ch_w;
    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    GPT_ERROR_RETURN(SSP_SUCCESS == err, err);
    p_ctrl->p_gpt_w = info.ptr;
    R_BSP_ModuleStart(&ssp_feature);

    ssp_feature.channel = ch_v;
    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    GPT_ERROR_RETURN(SSP_SUCCESS == err, err);
    p_ctrl->p_gpt_v = info.ptr;
    R_BSP_ModuleStart(&ssp_feature);

    ssp_feature.channel = ch_u;
    err = g_fmi_on_fmi.productFeatureGet(&ssp_feature, &info);
    GPT_ERROR_RETURN(SSP_SUCCESS == err, err);
    p_ctrl->p_gpt_u = info.ptr;
    R_BSP_ModuleStart(&ssp_feature);

    fmi_event_info_t event_info = { 0U};
    g_fmi_on_fmi.eventInfoGet(&ssp_feature, SSP_SIGNAL_GPT_COUNTER_OVERFLOW, &event_info);
    p_ctrl->irq = event_info.irq;

    return SSP_SUCCESS;
} /* End of function gpt_common_open */

static void gpt_set_pins(GPT_BASE_PTR p_gpt_base, uint32_t pins)
{
    gtior_t * p_reg = HW_GPT_GTIOCRegLookup(p_gpt_base, pins);
    HW_GPT_GTIOCInitialOutputSet(p_reg, GPT_OUTPUT_HIGH);
    HW_GPT_GTIOCCycleEndOutputSet(p_reg, GPT_OUTPUT_HIGH);
    HW_GPT_GTIOCCompareMatchOutputSet(p_reg, GPT_OUTPUT_TOGGLED);
    HW_GPT_GTIOCPinOutputEnable(p_reg, true);
}


/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/
/*******************************************************************************************************************//**
 * @brief  Powers on GPT timers, handles required initialization described in hardware manual. Implements timer_api_t::open.
 *
 * The Open function configures a three GPT channels, starts the channel_u, and provides a handle for use with the GPT API
 * Control and Close functions. This function must be called once prior to calling any other GPT API functions. After a
 * channel_u is opened, the Open function should not be called again for the same channel_u without first calling the
 * associated Close function.
 *
 * GPT hardware does not support one-shot functionality natively.  When using one-shot mode, the timer will be stopped
 * in an ISR after the requested period has elapsed.
 *
 * @retval SSP_SUCCESS           Initialization was successful and timer has started.
 * @retval SSP_ERR_ASSERTION     One of the following parameters is incorrect.  Either
 *                                 - p_cfg is NULL, OR
 *                                 - p_ctrl is NULL, OR
 *                                 - The channel_u requested in the p_cfg parameter is not available on the device
 *                                   selected in r_bsp_cfg.h.
 * @retval SSP_ERR_INVALID_ARGUMENT  One of the following parameters is invalid:
 *                                 - p_cfg->period: must be in the following range:
 *                                     - Lower bound: (1 / (PCLK frequency)
 *                                     - Upper bound: (0xFFFFFFFF * 1024 / (PCLK frequency))
 *                                 - p_cfg->p_callback not NULL, but ISR is not enabled. ISR must be enabled to
 *                                   use callback function.  Enable channel_u's overflow ISR in bsp_irq_cfg.h.
 *                                 - p_cfg->mode is ::TIMER_MODE_ONE_SHOT, but ISR is not enabled.  ISR must be
 *                                   enabled to use one-shot mode.  Enable channel_u's overflow ISR in bsp_irq_cfg.h.
 * @retval SSP_ERR_IN_USE        The channel_u specified has already been opened. No configurations were changed. Call
 *                               the associated Close function or use associated Control commands to reconfigure the
 *                               channel_u.
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel_u.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_Open (motor_ctrl_t * const p_ctrl, motor_cfg_t * const p_cfg)
{
    /*
    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0}};
    ssp_feature.channel = p_cfg->pwm_ch_u;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_GPT;
     */
    p_ctrl->p_cfg = p_cfg;
    p_ctrl->commutation_mode = p_cfg->commutation_mode;

    if (sf_callback == NULL)
    {
        /* Setup Motor Driver client callback function to be invoked every PWM cycle */
        sf_callback = p_cfg->callback;
    }

    /* Setup timers for each phase */
    gpt_common_open(p_ctrl);

    /* Calculate counter period */
    /* Read the current PCLK frequency from the clock module. */
    uint32_t pclk_freq_hz;
    g_cgc_on_cgc.systemClockFreqGet(CGC_SYSTEM_CLOCKS_PCLKD, &pclk_freq_hz);

    /* Convert PWM carrier frequency to PCLK counts so it can be set in hardware. */
    uint32_t period, freq;
    if (p_cfg->pwm_freq_KHz == 0)
        freq = 20;
    else
        freq = p_cfg->pwm_freq_KHz;
    period = pclk_freq_hz / (1000U * freq);
    period >>= 1;   /* For center-aligned PWM the cycle count is half of the edge-aligned */
    p_ctrl->pwm_period = period;
    p_ctrl->pwm_period_2 = period >> 1;

    /** Power on GPT before setting any hardware registers. Make sure the counter is stopped before setting mode
       register, PCLK divisor register, and counter register. */

    /* Hard-coded - half-cycle reload, center-aligned */
    motor_bldc_init(p_ctrl->p_gpt_u, p_cfg->pwm_ch_u, period, true, true);
    motor_bldc_init(p_ctrl->p_gpt_v, p_cfg->pwm_ch_v, period, true, true);
    motor_bldc_init(p_ctrl->p_gpt_w, p_cfg->pwm_ch_w, period, true, true);

    if (!irq_handler_set)
    {
        /* Setup only one PWM Carrier Interrupt */
        irq_handler_set = true;

        /* Setup PWM Interrupt */
        HW_GPT_InterruptEnable(p_ctrl->p_gpt_u, GPT_INT_TYPE_OVERFLOW);

        ssp_vector_info_t * p_vector_info;
        R_SSP_VectorInfoGet(p_ctrl->irq, &p_vector_info);
        NVIC_SetPriority(p_ctrl->irq, 1);
        *(p_vector_info->pp_ctrl) = p_ctrl;

        R_BSP_IrqStatusClear(p_ctrl->irq);
        NVIC_ClearPendingIRQ(p_ctrl->irq);
        NVIC_EnableIRQ(p_ctrl->irq);
    }

    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_A || p_ctrl->p_cfg->output_mode == PWM_OUT_COMPLEMENTARY)
    {
        gpt_set_pins(p_ctrl->p_gpt_u, GPT_GTIOCA);
        gpt_set_pins(p_ctrl->p_gpt_v, GPT_GTIOCA);
        gpt_set_pins(p_ctrl->p_gpt_w, GPT_GTIOCA);
    }
    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_B || p_ctrl->p_cfg->output_mode == PWM_OUT_COMPLEMENTARY)
    {
        gpt_set_pins(p_ctrl->p_gpt_u, GPT_GTIOCB);
        gpt_set_pins(p_ctrl->p_gpt_v, GPT_GTIOCB);
        gpt_set_pins(p_ctrl->p_gpt_w, GPT_GTIOCB);
    }
    /* Configure ADC conversion to be started by the PWM cycle */
    //HW_GPT_PWM_SetAdcStartRequest(p_ctrl->channels[0], pnt->con_delay);
    p_ctrl->p_gpt_u->GTADTRA = 0;               /* Delay */
    p_ctrl->p_gpt_u->GTINTAD |= 0x00010000;     /* channel B16*/

    // power on the POE circuit
    //HW_GPT_PWM_POE_PowerOn();
    // configures POE
    //HW_GPT_PWM_SetPoegA(0x30000030);

    /* Start all timer counters synchronously */
    p_ctrl->p_gpt_v->GTSTR = (uint32_t)((1 << p_cfg->pwm_ch_u) | (1 << p_cfg->pwm_ch_v) | (1 << p_cfg->pwm_ch_w));
    p_ctrl->open = 1;
    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_TimerOpen */


/*******************************************************************************************************************//**
 * @brief  Setups motor commutation. Implements timer_api_t::setup.
 *
 * @retval SSP_SUCCESS           Timer successfully stopped.
 * @retval SSP_ERR_ASSERTION     The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel_u is not opened.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_Setup (motor_ctrl_t * const p_ctrl, int32_t ecpr)
{
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    //GPT_ERROR_RETURN(gp_ctrls[p_ctrl->p_cfg->pwm_ch_u] == p_ctrl, SSP_ERR_NOT_OPEN);
#endif

    double ecpc;

    gpt_common_open(p_ctrl);

    /* Configure Phase Commutation */
    if (p_ctrl->p_cfg->pole_pairs != 0)
    {
        ecpc = (double)ecpr / (double)p_ctrl->p_cfg->pole_pairs;
        p_ctrl->ec_per_ecycle = (int32_t)(ecpc + 0.5);
    }
    else
    {
        ecpc = ecpr;
        p_ctrl->ec_per_ecycle = ecpr;
    }

    p_ctrl->counts2rad = (float)((M_PI+M_PI) / ecpc);

    /* Capture ADC offset at 0V before the PWM noise is enabled */
    p_ctrl->adc1_offs = p_ctrl->adc1_raw;
    p_ctrl->adc2_offs = p_ctrl->adc2_raw;
    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_Setup */

/*******************************************************************************************************************//**
 * @brief  Executes motor control algorithm. Implements motor_api_t::start.
 *
 * @retval SSP_SUCCESS           Timer successfully started.
 * @retval SSP_ERR_ASSERTION     The p_ctrl parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel_u is not opened.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_Control (motor_ctrl_t * const p_ctrl)
{
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    //GPT_ERROR_RETURN(gp_ctrls[p_ctrl->p_cfg->pwm_ch_u] == p_ctrl, SSP_ERR_NOT_OPEN);
#endif

    /* Execute the selected commutation algorithm */
    switch (p_ctrl->commutation_mode)
    {
    case COMMUTATION_FOC:            /* Field Oriented Control */
        p_ctrl->angle_rad = (float)(p_ctrl->position - p_ctrl->phase_origin) * p_ctrl->counts2rad;
        p_ctrl->angle_sin = sinf(p_ctrl->angle_rad);
        p_ctrl->angle_cos = cosf(p_ctrl->angle_rad);

        /* Field-Oriented Control */
        commutate_foc(p_ctrl);

        /* Space Vector Modulation. Result is +/-0.5 in p_ctrl->out_u/v/w */
        commutate_svm(p_ctrl, p_ctrl->foc_alpha, p_ctrl->foc_beta);
        update_pwm(p_ctrl);
        break;

    case COMMUTATION_SIN:            /* Space Vector Modulation only - no current loop */
        {
        /* Scale the output from +/-32767 to +/-1*/
        float pwm_out_q = p_ctrl->output_q * (1/32768.0f);
        float pwm_out_d = p_ctrl->output_d * (1/32768.0f);

        p_ctrl->angle_rad = (float)(p_ctrl->position - p_ctrl->phase_origin) * p_ctrl->counts2rad;
        p_ctrl->angle_sin = sinf(p_ctrl->angle_rad);
        p_ctrl->angle_cos = cosf(p_ctrl->angle_rad);

        /* Inverse Park Transformation */
        p_ctrl->calc_alpha= pwm_out_d * p_ctrl->angle_cos - pwm_out_q * p_ctrl->angle_sin;
        p_ctrl->calc_beta = pwm_out_q * p_ctrl->angle_cos + pwm_out_d * p_ctrl->angle_sin;

        /* Space Vector Modulation. Result is +/-0.5 in p_ctrl->out_u/v/w */
        commutate_svm(p_ctrl, p_ctrl->calc_alpha, p_ctrl->calc_beta);
        update_pwm(p_ctrl);

        /* For test */
        //commutate_foc(p_ctrl);
        }
        break;

    case COMMUTATION_TRAP:          /* Flux orientation is based on the hall sensors */
        p_ctrl->angle_rad = (float)(sector[p_ctrl->hall_state & 0x07] + 90) * (2.0f * M_PI / 360.0f);
        p_ctrl->calc_beta = sinf(p_ctrl->angle_rad) * (float)p_ctrl->output_q * (1/32768.0f);
        p_ctrl->calc_alpha = cosf(p_ctrl->angle_rad) * (float)p_ctrl->output_q * (1/32768.0f);
        commutate_svm(p_ctrl, p_ctrl->calc_alpha, p_ctrl->calc_beta);
        update_pwm(p_ctrl);
        break;

    case COMMUTATION_FORCED:         /* User-defined Flux angle at discrete values */
        commutate_set(p_ctrl);
        update_pwm(p_ctrl);

        /* For test */
        /*
        p_ctrl->angle_rad = (float)(p_ctrl->phase_sector) * (M_PI / 3.0f) + M_PI_2;   // 1 sector = 60 deg = PI/3
        p_ctrl->angle_sin = sinf(p_ctrl->angle_rad);
        p_ctrl->angle_cos = cosf(p_ctrl->angle_rad);
        commutate_foc(p_ctrl);
        */
        break;

    case COMMUTATION_EXT:           /* Individual Phase Voltages are set externally - by the host */
        update_pwm(p_ctrl);
        break;

    default:                        /* Each Phase voltage is user-defined */
        break;
    }
    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_Control */

/*******************************************************************************************************************//**
 * @brief  Sets counter value in provided p_value pointer. Implements timer_api_t::counterGet.
 *
 * @retval SSP_SUCCESS           Counter value read, p_value is valid.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel_u is not opened.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_CounterGet (motor_ctrl_t * const p_ctrl, int32_t * const p_value)
{
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_value);
    //GPT_ERROR_RETURN(gp_ctrls[p_ctrl->p_cfg->pwm_ch_u] == p_ctrl, SSP_ERR_NOT_OPEN);
#endif

    /** Read counter value */
    *p_value = (int32_t)HW_GPT_CounterGet(p_ctrl->p_gpt_u);
    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_CounterGet */

 /*******************************************************************************************************************//**
 * @brief  Get timer information and store it in provided pointer p_info.
 * Implements timer_api_t::infoGet.
 *
 * @retval SSP_SUCCESS           Period, count direction, frequency, and status value written to caller's
 *                               structure successfully.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_info parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel_u is not opened.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_InfoGet (motor_ctrl_t * const p_ctrl, motor_info_t * const p_info)
{
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure parameters are valid. */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_info);
    //GPT_ERROR_RETURN(gp_ctrls[p_ctrl->p_cfg->pwm_ch_u] == p_ctrl, SSP_ERR_NOT_OPEN);
#endif
    SSP_PARAMETER_NOT_USED(p_ctrl);
    SSP_PARAMETER_NOT_USED(p_info);

    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_InfoGet */

/*******************************************************************************************************************//**
 * @brief      Stops counter, disables interrupts, disables output pins, and clears internal driver data.
 *
 * @retval     SSP_SUCCESS          Successful close.
 * @retval     SSP_ERR_ASSERTION    The parameter p_ctrl is NULL.
 * @retval     SSP_ERR_NOT_OPEN     The channel_u is not opened.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_Close (motor_ctrl_t * const p_ctrl)
{
    ssp_err_t err = SSP_SUCCESS;
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Make sure channel_u is open.  If not open, return. */
    SSP_ASSERT(NULL != p_ctrl);
    //GPT_ERROR_RETURN(gp_ctrls[p_ctrl->p_cfg->pwm_ch_u] == p_ctrl, SSP_ERR_NOT_OPEN);
#endif

    p_ctrl->open = 0;

    /** Cleanup. Disable interrupts, stop counter, and disable output. */
    HW_GPT_InterruptDisable(p_ctrl->p_gpt_u);
    HW_GPT_CounterStartStop(p_ctrl->p_gpt_u, GPT_STOP);
    HW_GPT_CounterStartStop(p_ctrl->p_gpt_v, GPT_STOP);
    HW_GPT_CounterStartStop(p_ctrl->p_gpt_w, GPT_STOP);

    /*
    IRQn_Type irq = HW_GPT_IRQGet(p_ctrl->p_cfg->pwm_ch_u);
    if (BSP_MAX_NUM_IRQn > irq)
    {
        NVIC_DisableIRQ(irq);
    }
     */
    gtior_t * p_reg;

    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_A || p_ctrl->p_cfg->output_mode == PWM_OUT_COMPLEMENTARY)
    {
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_u, GPT_GTIOCA);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_v, GPT_GTIOCA);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_w, GPT_GTIOCA);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
    }
    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_B || p_ctrl->p_cfg->output_mode == PWM_OUT_COMPLEMENTARY)
    {
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_u, GPT_GTIOCB);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_v, GPT_GTIOCB);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
        p_reg = HW_GPT_GTIOCRegLookup(p_ctrl->p_gpt_w, GPT_GTIOCB);
        HW_GPT_GTIOCPinOutputEnable(p_reg, false);
    }

    return err;
} /* End of function R_Motor_BLDC_Close */

/*******************************************************************************************************************//**
 * @brief      Sets driver version based on compile time macros.
 *
 * @retval     SSP_SUCCESS          Successful close.
 * @retval     SSP_ERR_ASSERTION    The parameter p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t R_Motor_BLDC_VersionGet (ssp_version_t * const p_version)
{
#if R_MOTOR_BLDC_CFG_PARAM_CHECKING_ENABLE
    /** Verify parameters are valid */
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = g_motor_bldc_version.version_id;

    return SSP_SUCCESS;
} /* End of function R_Motor_BLDC_VersionGet */
/** @} (end addtogroup Motor) */

/*********************************************************************************************************************//**
 * Private Functions
 **********************************************************************************************************************/

