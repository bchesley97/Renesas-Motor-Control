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
#include "r_motor_api_changed.h"
#include "r_motor_bldc.h"
#include "r_motor_bldc_cfg.h"
#include "../src/driver/r_gpt/hw/hw_gpt_private.h"
#include "../src/driver/r_motor_bldc/r_motor_bldc_private_api.h"
#include "../src/driver/r_motor_bldc/hw/hw_motor_bldc_private.h"
#include "r_gpt.h"
#include "common_data.h"

//global variables
mtr_added_ctrl_t mtr_pattern_ctrl;
mtr_added_ctrl_t *p_mtr_pattern_ctrl = &mtr_pattern_ctrl;
trap_phase_state_t trap_pattern[6] =       {{PSTAT_LOW, PSTAT_HIGH,PSTAT_PWM }, //trapezoidal commutation PWM pattern
                                            {PSTAT_PWM, PSTAT_HIGH,PSTAT_LOW },
                                            {PSTAT_PWM, PSTAT_LOW, PSTAT_HIGH},
                                            {PSTAT_LOW, PSTAT_PWM, PSTAT_HIGH},
                                            {PSTAT_HIGH,PSTAT_PWM, PSTAT_LOW },
                                            {PSTAT_HIGH,PSTAT_LOW, PSTAT_PWM }};

//    PIN_DUTY_PWM         = 0x00000001,  //1 in bit 0 to keep UD bit = 1 (up counting)
//    PIN_DUTY_100         = 0x03020001,
//    PIN_DUTY_0           = 0x02020001

phase_pin_ctrl_t pins_ctrl[] =  { {{PIN_DUTY_0},    {PIN_DUTY_100}, {PIN_DUTY_PWM}},
                                {{PIN_DUTY_PWM},  {PIN_DUTY_100},  {PIN_DUTY_0}},
                                {{PIN_DUTY_PWM},  {PIN_DUTY_0},     {PIN_DUTY_100}},
                                {{PIN_DUTY_0},    {PIN_DUTY_PWM}, {PIN_DUTY_100}},
                                {{PIN_DUTY_100},  {PIN_DUTY_PWM}, {PIN_DUTY_0}},
                                {{PIN_DUTY_100},  {PIN_DUTY_0},   {PIN_DUTY_PWM}} };

//phase_pin_ctrl_t pins_ctrl[] =  { {{0x02020001},    {0x03020001}, {0x00000001}},
//                                {{0x00000001},  {0x03020001},  {0x02020001}},
//                                {{0x00000001},  {0x0202001},     {0x03020001}},
//                                {{0x02020001},    {0x00000001}, {0x03020001}},
//                                {{0x03020001},  {0x00000001}, {0x02020001}},
//                                {{0x03020001},  {0x02020001},   {0x00000001}} };

phase_pin_ctrl_t *p_pins_ctrl = &pins_ctrl[0];

uint32_t pin_ctrl_u[] = {0x02020001, 0x00000001, 0x00000001, 0x02020001, 0x03020001, 0x03020001};
uint32_t pin_ctrl_v[] = {0x03020001, 0x03020001, 0x02020001, 0x00000001, 0x00000001, 0x02020001};
uint32_t pin_ctrl_w[] = {0x00000001, 0x02020001, 0x03020001, 0x03020001, 0x02020001, 0x00000001};


uint32_t dtc_vect_table[] = { 0x20010000};

const uint32_t * table_address = 0x20000000;
extern motor_instance_t const *g_motors[16];

//global variables for setting DTC transfer for changing comparator input
uint8_t cmpctl_coe_0 = 0b10011000; //COE bit is bit 1
//IVCMP0 is U phase pin, IVCMP1 is V phase pin, IVCMP2 is W phase pin
uint8_t zero = 0x00;
uint8_t cmpsel_regs[] = {0x01, 0x04, 0x02, 0x01, 0x04, 0x02}; //input to comparator register values
uint8_t cmpctl_coe_1 = 0b10011010; //same as cmpctl_coe_0 except coe bit is 1
uint32_t clear_ielsrn = 0x01000057; //0x057 is ACMP_HS0 source, could possible change


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
static unsigned char output_disable_channel = 0; //determines which channel of POEG is mapped to a u,v,w channel




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

//function to change duty cycle of waveform
void change_pwm_duty(uint32_t duty_cycle_percent)
{


    if(duty_cycle_percent > 100)
    {
        return; //basic parameter checking
    }

    uint32_t duty_value_ticks = g_motors[1]->p_ctrl->pwm_period - (uint32_t)((g_motors[1]->p_ctrl->pwm_period * duty_cycle_percent)/100.0f);

    motor_ctrl_t * const p_ctrl = g_motors[1]->p_ctrl;

    //this function changes the pwm of all three GPT timers used (for each phase)
    p_ctrl->p_gpt_u->GTDTCR_b.TDE = 0; //set this value zero so new GTDVU value updated when TDE bit set to 1
    p_ctrl->p_gpt_u->GTCCRC = duty_value_ticks;
    p_ctrl->p_gpt_u->GTDTCR_b.TDE = 1; //this will place new GTDVU value automatically

    p_ctrl->p_gpt_v->GTDTCR_b.TDE = 0; //set this value zero so new GTDVU value updated when TDE bit set to 1
    p_ctrl->p_gpt_v->GTCCRC = duty_value_ticks;
    p_ctrl->p_gpt_v->GTDTCR_b.TDE = 1; //this will place new GTDVU value automatically

    p_ctrl->p_gpt_w->GTDTCR_b.TDE = 0; //set this value zero so new GTDVU value updated when TDE bit set to 1
    p_ctrl->p_gpt_w->GTCCRC = duty_value_ticks;
    p_ctrl->p_gpt_w->GTDTCR_b.TDE = 1; //this will place new GTDVU value automatically

}


void set_ol_speed_rpms(float rpms)
{
    //set the speed of the motor in open loop control via parameter in terms of rpms
    uint32_t interrupt_rate_hz = 20000;
    float cycles_per_sec = (rpms/(10.0f));
    uint32_t count = (uint32_t)((float)(interrupt_rate_hz)/cycles_per_sec);
    p_mtr_pattern_ctrl->vel_accel.velocity = count;
}

//each timer for u,v, and w phases are passed in. center aligned and half cycle reload hard coded to true (not sure why)
static void motor_bldc_init(R_GPTA0_Type *p_gpt, uint16_t ch, uint32_t period, bool center_aligned, bool half_cycle_reload)
{
    //GTBER is general PWM Timer Buffer Enable Register
    HW_MTR_RegisterInit(p_gpt, ch, period);
    p_gpt->GTBER_b.CCRA = 1;   /* Enable single buffer operation, CCRA<->CCRC */
  //  p_gpt->GTBER_b.CCRB = 1;   /* Enable single buffer operation, CCRB<->CCRE */

    p_gpt->GTBER_b.ADTTA = 1; //transfer at crest for triangle waves, otherwise transfer over/under flow depending on counting direction
 //   p_gpt->GTBER_b.ADTTB = 1; //transfer at crest

    if (center_aligned) //center aligned?
    {                                                                                       ///< Triangle wave PWM mode 1 (16-bit transfer at crest) (single or double buffer)
        HW_GPT_ModeSet(p_gpt, GPT_MODE_TW_PWM_1);  ///< Triangle wave PWM mode 2 (16-bit transfer at trough) (single or double buffer)
    }
    else
    {
        HW_GPT_ModeSet(p_gpt, GPT_MODE_PERIODIC); //periodic timer counter (normal) rather than PWM operation
    }

}

volatile int32_t tu, tv, tw;

static void update_pwm(motor_ctrl_t * p_ctrl)
{
    /* Convert output (+0.5 .. -0.5) to 0 .. Period Count */
    tu = (int32_t)((float)p_ctrl->pwm_period * p_ctrl->out_u) + (int32_t)p_ctrl->pwm_period_2; //out's are the phase voltages
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


uint32_t dead_time_ticks=0;
/* set pins of both A and B channels for the timer in complementary mode
 * A is regular output and B is the complement of B
 *
 * param: dead time in us from ctrl struct
 */
static void gpt_set_pins_comp(GPT_BASE_PTR p_gpt_base, float *dead_time_ns) //this function doesnt seem to take into account the complementary PWM setting. changing to account for this (probably dont need for actual implementation since board creates its own complementary pwm signal)
{

    /*** Set automatic dead time ***/

    //transform dead time value in us to a period value in terms of timer ticks

    //get pclkd clock frequency for dead timer calculation
    uint32_t pclkd_freq_hz = 0;
    g_cgc.p_api->systemClockFreqGet(CGC_SYSTEM_CLOCKS_PCLKD,  &pclkd_freq_hz);


    //calculate dead time value
    float pclkd_freq_mhz = ((pclkd_freq_hz)/1000000); //dead time value = frequency of clock * dead time in us * 10^-6 (because of us units)

    float dead_time = *dead_time_ns;
    dead_time_ticks = (uint32_t)((2*pclkd_freq_mhz * dead_time)/1000); //need to multiply by two since using triangle waveform modes

    p_gpt_base->GTCCRB = 0; //reset to compare value for B to zero, will be taken care of by the dead time registers
    p_gpt_base->GTCCRE = 0; //same deal as above
    p_gpt_base->GTDVU = dead_time_ticks;
    p_gpt_base->GTDVD = dead_time_ticks;

    p_gpt_base->GTDTCR = 0x00000101; //use GTDVU and GTDVD to set compare match value for negative phase waveform with auto dead time in GTCCRB

    uint32_t pins = GPT_GTIOCA; //set channel A
    //p_gpt_base_pos is channel that isnt inverted, the comp parameter is the complemented PWM timer
    gtior_t * p_reg = HW_GPT_GTIOCRegLookup(p_gpt_base, pins);


    HW_GPT_GTIOCInitialOutputSet(p_reg, GPT_OUTPUT_LOW);
    HW_GPT_GTIOCCycleEndOutputSet(p_reg, GPT_OUTPUT_LOW);
    HW_GPT_GTIOCCompareMatchOutputSet(p_reg, GPT_OUTPUT_TOGGLED);
    HW_GPT_GTIOCPinLevelStoppedSet(p_reg, GPT_OUTPUT_LOW); //when stopped output low
    HW_GPT_GTIOCPinOutputEnable(p_reg, true);

    //initialize complementary timer channel (B)
    pins = GPT_GTIOCB;
    p_reg = HW_GPT_GTIOCRegLookup(p_gpt_base, pins);


    HW_GPT_GTIOCInitialOutputSet(p_reg, GPT_OUTPUT_HIGH);
    HW_GPT_GTIOCCycleEndOutputSet(p_reg, GPT_OUTPUT_HIGH);
    HW_GPT_GTIOCCompareMatchOutputSet(p_reg, GPT_OUTPUT_TOGGLED);
    HW_GPT_GTIOCPinLevelStoppedSet(p_reg, GPT_OUTPUT_LOW); //when stopped output low
    HW_GPT_GTIOCPinOutputEnable(p_reg, true);

}


static inline void enable_dtc_elc(motor_ctrl_t * const p_ctrl)
{

    /*** Configure the ELC to trigger DTC on PWM interrupt ***/
//
    R_PMISC->PWPR_b.BOWI = 0; //enable writes
    R_PMISC->PWPR_b.PFSWE = 1;

    R_PFS->P202PFS_b.EOFR = 0b01; //signal event on rising edge on P202

    R_PMISC->PWPR_b.PFSWE = 0;
    R_PMISC->PWPR_b.BOWI = 1; //disable writes

//the register used for ELSRnRCO[] does not matter since just triggering DTC. Make sure not to use a peripheral function that actually needs to be linked (not just dtc)
    R_ELC->ELSRnRC0[15].ELSRn_b.ELS = 0x95; //p2 event
    R_ELC->ELCR_b.ELCON = 1; //enable elc for all events


    /*** Configure the DTC ***/
    //need to configure vector table in sram area

    uint32_t address = 0x20040000;
    volatile uint32_t * dtc_transfer_info_ptr = (uint32_t*)0x20040000;

    uint32_t  sar_u = (uint32_t)&pin_ctrl_u[0];
    uint32_t  sar_v = (uint32_t)&pin_ctrl_v[0];
    uint32_t  sar_w = (uint32_t)&pin_ctrl_w[0];

    uint32_t dar_u = (uint32_t)&p_ctrl->p_gpt_u->GTUDDTYC;
    uint32_t dar_v = (uint32_t)&p_ctrl->p_gpt_v->GTUDDTYC;
    uint32_t dar_w = (uint32_t)&p_ctrl->p_gpt_w->GTUDDTYC;

    uint32_t *dtc_vector_table_ptr = (uint32_t*)(0x200FE000);
    *dtc_vector_table_ptr = 0x20040000;
    dtc_vector_table_ptr = dtc_vector_table_ptr+1;
    *dtc_vector_table_ptr = 0x20040000;

    dtc_vector_table_ptr = dtc_vector_table_ptr+1;
   *dtc_vector_table_ptr = 0;
   dtc_vector_table_ptr = dtc_vector_table_ptr+3;
   *dtc_vector_table_ptr = 0x20040000;

    R_DTC->DTCST = 0;

    //Set ICU.IELSRn.IELS bit to 0. disable interrupt in NVIC and do the following:
    //1. set DTCCR.RRS bit to 0
    R_DTC->DTCCR_b.RRS = 0;

    //2. allocate transfer information (MRA,MRB,SAR,DAR,CRA,CRB) in the data area

    /* transfer information for first timer (u timer, first transfer) */

    //place configuration values into vector table

     /* Configuration values for first transfer are :
     *  MRA: increment source address, word transfer, repeat transfer mode
     *  MRB: fixed destination address, transfer source as repeat area, generate interrupt when specified data transfer complete, continuous chain transfer, enable chain transfer
     *  SAR: table
     *  DAR: register controlling duty cycle of respective timer
     *  CRA: CRAH-6, CRAL-6, since repeat mode and 6 entries in the source table
     *  DTCCR: rrs bit is set even though transfer info alway read since chained
     */


    *dtc_transfer_info_ptr = (uint32_t)((0b01101000 << 24) | (0b10010000 <<16) | 0x0000);
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = sar_u;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = dar_u;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = (uint32_t)(((6 << 8) | 6) << 16);
    dtc_transfer_info_ptr++; //increment pointer

    /* Configuration values for second transfer are : (same as first)
    *  MRA: increment source address, word transfer, repeat transfer mode
    *  MRB: fixed destination address, transfer source as repeat area, generate interrupt when specified data transfer complete, continuous chain transfer, enable chain transfer
    *  SAR: table
    *  DAR: register controlling duty cycle of respective timer
    *  CRA: CRAH-6, CRAL-6, since repeat mode and 6 entries in the source table
    *  DTCCR: rrs bit is set even though transfer info alway read since chained
    */    /* transfer information for second timer (v timer, second transfer) (all info is the same except destination register*/

    *dtc_transfer_info_ptr = (uint32_t)((0b01101000 << 24) | (0b10010000 <<16) | 0x0000);
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = sar_v;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = dar_v;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = (uint32_t)(((6 << 8) | 6) << 16);
    dtc_transfer_info_ptr++; //increment pointer


    /*transfer information for third timer (w timer, third and last transfer) all settings are the same except for the mrb.chne = 0 and mrb.disel = 0*/
    /* Configuration values for first transfer are :
    *  MRA: increment source address, word transfer, repeat transfer mode
    *  MRB: fixed destination address, transfer source as repeat area, generate interrupt when specified data transfer complete, continuous chain transfer, disable chain transfer
    *  SAR: table
    *  DAR: register controlling duty cycle of respective timer
    *  CRA: CRAH-6, CRAL-6, since repeat mode and 6 entries in the source table
    *  DTCCR: rrs bit is set even though transfer info alway read since chained
    */

    *dtc_transfer_info_ptr = (uint32_t)((0b01101000 << 24) | (0b10010000 << 16) | 0x0000);
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = sar_w;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = dar_w;
    dtc_transfer_info_ptr++; //increment pointer
    *dtc_transfer_info_ptr = (uint32_t)(((6 << 8) | 6) << 16);
    dtc_transfer_info_ptr++; //increment pointer


//
//    /*** now set up DTC (still as the same chain transfer) to change the comparator input to the floating pin ***/
//    /* the following steps are taken to change the input select register, just occurs via DTC
//     *  1. Set the CMPCTL.COE bit to 0.
//        2. Set the CMPSEL0 register to 0000 0000b.
//        3. Set a new value in the CMPSEL[3:0] bits, with 1 set in only one of the bits.
//        4. Wait for the input switching stabilization wait time (200 ns).
//        5. Set the CMPCTL.COE bit to 1.
//        6. Clear the IR flag in the IELSRn register to clear the interrupt status.
//     */
//
//
//    *dtc_transfer_info_ptr = (uint32_t)((0b01000000 << 24) | (0b100000000 << 16) | 0x0000);
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &cmpctl_coe_0;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &R_COMP0->CMPCTL;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t)(((1 << 8) | 1) << 16);
//    dtc_transfer_info_ptr++; //increment pointer
//
//
//    *dtc_transfer_info_ptr = (uint32_t)((0b01000000 << 24) | (0b100000000 << 16) | 0x0000);
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &zero;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &R_COMP0->CMPSEL0;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t)(((1 << 8) | 1) << 16);
//    dtc_transfer_info_ptr++; //increment pointer
//
//
//    *dtc_transfer_info_ptr = (uint32_t)((0b01000000 << 24) | (0b100000000 << 16) | 0x0000);
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &cmpsel_regs[0];
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &R_COMP0->CMPSEL0;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t)(((6 << 8) | 6) << 16);
//    dtc_transfer_info_ptr++; //increment pointer
//
//
//    *dtc_transfer_info_ptr = (uint32_t)((0b01000000 << 24) | (0b100000000 << 16) | 0x0000);
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &cmpctl_coe_1;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &R_COMP0->CMPCTL;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t)(((1 << 8) | 1) << 16);
//    dtc_transfer_info_ptr++; //increment pointer
//
//
////last transfer, disable chain transfer
//    *dtc_transfer_info_ptr = (uint32_t)((0b01000000 << 24) | (0b000000000 << 16) | 0x0000);
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &clear_ielsrn;
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t) &R_ICU->IELSRn[4];
//    dtc_transfer_info_ptr++; //increment pointer
//    *dtc_transfer_info_ptr = (uint32_t)(((1 << 8) | 1) << 16);
//    //dtc_transfer_info_ptr++; //increment pointer


    //3. set transfer info start addresses in DTC vector table
    R_DTC->DTCVBR = (0x200FE000); //lower ten bits should be zero. arbitrary position

    //4. set DTCCR.RRS bit to 1 enables skipping of second and subsequent transfer info read cycles for continuous DTC activation from same interrupt

    R_DTC->DTCCR_b.RRS = 1; //doesnt really matter since the transfer info is always ready when its a chain transfer


    //5. set ICU.IELSRn.DTCE bit to 1. set ICU.IELSRn.IELS as the interrupt sources that trigger DTC. the interrupts must be enabled in NVIC

    R_ICU->IELSRn_b[5].IELS = 0x95; //[1] is the number of events currently in the ICU. 0x95 is the IOPORT2_event trigger. value at [0] is PWM interrupt
    R_ICU->IELSRn_b[5].DTCE = 1;

    //6. set enable bit for activation source interrupts to 1. when source interrupt is generated, DTC is activated
    ///not using interrupts so nothing to do here.

    //7. set the start module bit to 1 (DTCST.DTCST to 1)
    ///this bit is set in the pwm control thread when control switches from open loop to closed loop

}

static inline void enable_pgas()
{

    //enable writing to the ADC
    R_MSTP->MSTPCRD_b.MSTPD16 = 0; //enable writing to adc's registers

    /* enable the three PGAs for the three BEMF waveforms */
    R_S12ADC0->ADPGACR_b.P000SEL0 = 0; //do not output the signal in a path bypassing pga amplifier
    R_S12ADC0->ADPGACR_b.P000SEL1 = 1; //output singla in a path through pga amplifier
    R_S12ADC0->ADPGACR_b.P000ENAMP = 1; //use pga amplifier
    R_S12ADC0->ADPGACR_b.P000GEN = 1; //enable gain setting

    R_S12ADC0->ADPGACR_b.P001SEL0 = 0; //do not output the signal in a path bypassing pga amplifier
    R_S12ADC0->ADPGACR_b.P001SEL1 = 1; //output singla in a path through pga amplifier
    R_S12ADC0->ADPGACR_b.P001ENAMP = 1;//use pga amplifier
    R_S12ADC0->ADPGACR_b.P001GEN = 1; //enable gain setting

    R_S12ADC0->ADPGACR_b.P002SEL0 = 0; //do not output the signal in a path bypassing pga amplifier
    R_S12ADC0->ADPGACR_b.P002SEL1 = 1; //output singla in a path through pga amplifier
    R_S12ADC0->ADPGACR_b.P002ENAMP = 1;//use pga amplifier
    R_S12ADC0->ADPGACR_b.P002GEN = 1; //enable gain setting

    R_S12ADC0->ADPGAGS0_b.P000GAIN = 0b000; // gain of 2
    R_S12ADC0->ADPGAGS0_b.P001GAIN = 0b000; // gain of 2
    R_S12ADC0->ADPGAGS0_b.P002GAIN = 0b000; // gain of 2


}


//initializes DAC12 channel 0 to be used in BEMF sensing
static inline void enable_bemf_dac()
{
    //to access the DACs registers, need to cancel module stop state
    R_MSTP->MSTPCRD_b.MSTPD20 = 0; //enable writing to dac's registers

    //select pin to use as DAC0
    ///P014 is configured in the synergy configuration window for DAC0 operation

    // measured value for bemf is 0.19V
    // DAC equation: V= dac/4096 * 3.3. Plugging in V = 0.19*2 (multiply by two for pga gain) yields DAC = 471.66 ~= 472 = 0x.

    //12 bit digital value to be converted to an analog value and used as input to the comparator
    R_DAC->DADRn[0] = 0x01D8; //DADR[0] is DADR0. value outputted from dac (0.19V * 2)
    R_DAC->DADPR_b.DPSEL = 0; //right justified 12 bit value
    R_DAC->DACR_b.DAOE0 = 1; //enable D/A conversion of Channel 0, analog output is also enables (wont need since being used in internal comparator)
}


//this function sets up the high speed analog comparator to compare the back emf phase voltages
//to a voltage set by an internal DAC
static inline void enable_bemf_comps()
{


    //enable DAC for comparator reference input
    enable_bemf_dac();

    enable_pgas(); //enable pga comparator input

    /** Since the PGAs are needed to detect the back emf waveform, need to use three different comparators with three pga inputs ***/

    //need to disable module stop function
    R_MSTP->MSTPCRD_b.MSTPD28 = 0; //enable writing to comparator registers (comparator 0)
    R_MSTP->MSTPCRD_b.MSTPD27 = 0; //enable writing to comparator registers (comparator 1)
    R_MSTP->MSTPCRD_b.MSTPD26 = 0; //enable writing to comparator registers (comparator 2)


    /** U is IVCMP0, V is IVCMP1, W is IVCMP2 **/

    //AN000, AN001, and AN002 are already configured with the configuration view

    /*** Configuration for comparator 0 ***/

    //configure input to comparator
    R_COMP0->CMPSEL0_b.CMPSEL = 0b1000; //select PGA output as inpput to this comparator
    //configure comparator reference, DAC12 channel 0, on pin P014
    R_COMP0->CMPSEL1_b.CRVS = 0b1000; //IVREF3 corresponds to DAC Channel. this configuration register is static

    //in order to change the above crvs bits, need to set CMPCTL.COE to zero, then set the CMPSEL1 reg to 0, set new CRVS value, wait 200 ns, set CMPCTL.COE to 1, clear IR flag i IELSRn register to clear interrupt status
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 200 ns, might create latency

    //configure output activation level
    R_COMP0->CMPMON_b.CMPMON = 1; //comparator output is active high (also configure for rising edge interrupt)

    //writing to control register
    R_COMP0->CMPCTL_b.CEG = 0b11; //detect both edges since the back emf waveform will be crossing with both positive and negative slope
    R_COMP0->CMPCTL_b.CSTEN = 0; //output through the edge selector
    R_COMP0->CMPCTL_b.CDFS = 0b11; //use noise filter sampling frequency of pclkb/2^5
    R_COMP0->CMPCTL_b.COE = 1; //enable comparator output
    R_COMP0->CMPCTL_b.HCMPON = 1; //enable operation (enables input to comparator pins)

    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 300 ns for comparator output

    /*** Configuration for comparator 1 ***/
    //configure input to comparator
    R_COMP1->CMPSEL0_b.CMPSEL = 0b1000; //select PGA output as inpput to this comparator
    //configure comparator reference, DAC12 channel 0, on pin P014
    R_COMP1->CMPSEL1_b.CRVS = 0b1000; //IVREF3 corresponds to DAC Channel. this configuration register is static

    //in order to change the above crvs bits, need to set CMPCTL.COE to zero, then set the CMPSEL1 reg to 0, set new CRVS value, wait 200 ns, set CMPCTL.COE to 1, clear IR flag i IELSRn register to clear interrupt status
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 200 ns, might create latency

    //configure output activation level
    R_COMP1->CMPMON_b.CMPMON = 1; //comparator output is active high (also configure for rising edge interrupt)

    //writing to control register
    R_COMP1->CMPCTL_b.CEG = 0b11; //detect both edges since the back emf waveform will be crossing with both positive and negative slope
    R_COMP1->CMPCTL_b.CSTEN = 0; //output through the edge selector
    R_COMP1->CMPCTL_b.CDFS = 0b11; //use noise filter sampling frequency of pclkb/2^5
    R_COMP1->CMPCTL_b.COE = 1; //enable comparator output
    R_COMP1->CMPCTL_b.HCMPON = 1; //enable operation (enables input to comparator pins)

    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 300 ns for comparator output


    /*** Configuration for comparator 2 ***/
    //configure input to comparator
    R_COMP2->CMPSEL0_b.CMPSEL = 0b1000; //select PGA output as inpput to this comparator
    //configure comparator reference, DAC12 channel 0, on pin P014
    R_COMP2->CMPSEL1_b.CRVS = 0b1000; //IVREF3 corresponds to DAC Channel. this configuration register is static

    //in order to change the above crvs bits, need to set CMPCTL.COE to zero, then set the CMPSEL1 reg to 0, set new CRVS value, wait 200 ns, set CMPCTL.COE to 1, clear IR flag i IELSRn register to clear interrupt status
    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 200 ns, might create latency

    //configure output activation level
    R_COMP2->CMPMON_b.CMPMON = 1; //comparator output is active high (also configure for rising edge interrupt)

    //writing to control register
    R_COMP2->CMPCTL_b.CEG = 0b11; //detect both edges since the back emf waveform will be crossing with both positive and negative slope
    R_COMP2->CMPCTL_b.CSTEN = 0; //output through the edge selector
    R_COMP2->CMPCTL_b.CDFS = 0b11; //use noise filter sampling frequency of pclkb/2^5
    R_COMP2->CMPCTL_b.COE = 1; //enable comparator output
    R_COMP2->CMPCTL_b.HCMPON = 1; //enable operation (enables input to comparator pins)

    R_BSP_SoftwareDelay(1,BSP_DELAY_UNITS_MICROSECONDS); //wait 300 ns for comparator output

    /** set up interrupt for comparator. the first phase will be detecting the V floating pin */
    //not currently enabling irq in NVIC, just in ICU to see if the flag is being set
    uint8_t comparator_irq = 3;
    R_ICU->IELSRn[comparator_irq] = ELC_EVENT_COMP_HS_1; //this will change as the commutation pattern changes

//    NVIC_SetPriority(comparator_irq, 1);
//    R_BSP_IrqStatusClear(comparator_irq);
//    NVIC_ClearPendingIRQ(comparator_irq);
//    NVIC_EnableIRQ(comparator_irq);

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
  // period >>= 1;   /* For center-aligned PWM the cycle count is half of the edge-aligned */
    p_ctrl->pwm_period = period;
    p_ctrl->pwm_period_2 = period >> 1;

    /** Power on GPT before setting any hardware registers. Make sure the counter is stopped before setting mode
       register, PCLK divisor register, and counter register. */

    /* Hard-coded - half-cycle reload, center-aligned */
    motor_bldc_init(p_ctrl->p_gpt_u, p_cfg->pwm_ch_u, period, true, true);
    motor_bldc_init(p_ctrl->p_gpt_v, p_cfg->pwm_ch_v, period, true, true);
    motor_bldc_init(p_ctrl->p_gpt_w, p_cfg->pwm_ch_w, period, true, true);

    //set the initial values for open loop control
    p_mtr_pattern_ctrl->vel_accel.velocity = 1000; //velocity is in terms of PWM counts, more counts the slower. Starting at 500 counts
    p_mtr_pattern_ctrl->vel_accel.acceleration = 2; //initial rate to accelerate at
    p_mtr_pattern_ctrl->ctrl_type = OPEN_LOOP_CONTROL; //initially starting, use open loop control till back EMF strong enough to sense with comparator
    p_mtr_pattern_ctrl->p_trap_pattern = &trap_pattern[0];

    if(p_cfg->output_mode ==  PWM_OUT_COMPLEMENTARY) //not really related to dtc or elc but makes sure proper dtc is initialized
    {

        //setup interrupt for only one timer out of the timers used for trapezoidal commutation
        if (!irq_handler_set)
        {
            /* Setup only one PWM Carrier Interrupt */
            irq_handler_set = true;

             /* Setup PWM Interrupt */
            HW_GPT_InterruptEnable(p_ctrl->p_gpt_u, GPT_INT_TYPE_OVERFLOW);

            /* enable interrupt skipping (count crests, skip 2 interrupts before firing) */
            p_ctrl->p_gpt_u->GTITC = 0x00000241;

            ssp_vector_info_t * p_vector_info;
            R_SSP_VectorInfoGet(p_ctrl->irq, &p_vector_info);
            NVIC_SetPriority(p_ctrl->irq, 1);
            *(p_vector_info->pp_ctrl) = p_ctrl;

            R_BSP_IrqStatusClear(p_ctrl->irq);
            NVIC_ClearPendingIRQ(p_ctrl->irq);
            NVIC_EnableIRQ(p_ctrl->irq);

            enable_bemf_comps();

            /* enable the DTC linked with the ELC for closed loop control */
           enable_dtc_elc(p_ctrl); //enable dtc only for one motor

        }

    }

    /**** Independent modes ****/
    if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_A)
    {
        gpt_set_pins(p_ctrl->p_gpt_u, GPT_GTIOCA);
        gpt_set_pins(p_ctrl->p_gpt_v, GPT_GTIOCA);
        gpt_set_pins(p_ctrl->p_gpt_w, GPT_GTIOCA);
    }
    else if (p_ctrl->p_cfg->output_mode == PWM_OUT_INDEPENDENT_B)
    {
        gpt_set_pins(p_ctrl->p_gpt_u, GPT_GTIOCB);
        gpt_set_pins(p_ctrl->p_gpt_v, GPT_GTIOCB);
        gpt_set_pins(p_ctrl->p_gpt_w, GPT_GTIOCB);
    }

    /*** Complementary mode ***/
    if (p_ctrl->p_cfg->output_mode == PWM_OUT_COMPLEMENTARY) //only initialize and start the trapezoidal commutation timers
    {
        float dead_time_ns = 1000; //1 us dead time. need this value for when the motor starts running at high RP
        gpt_set_pins_comp(p_ctrl->p_gpt_u, &dead_time_ns); //second argument is in NANOSECONDS, since dealing with small duty cycles
        gpt_set_pins_comp(p_ctrl->p_gpt_v, &dead_time_ns);
        gpt_set_pins_comp(p_ctrl->p_gpt_w, &dead_time_ns);



        change_pwm_duty(1); //change duty cycle to one percent

        //set registers such that the first waveform displayed via PWM is the FIRST waveform
        p_ctrl->p_gpt_u->GTUDDTYC = pin_ctrl_u[0];
        p_ctrl->p_gpt_v->GTUDDTYC = pin_ctrl_v[0];
        p_ctrl->p_gpt_w->GTUDDTYC = pin_ctrl_w[0];

        /* Configure ADC conversion to be started by the PWM cycle */
        //HW_GPT_PWM_SetAdcStartRequest(p_ctrl->channels[0], pnt->con_delay);
        p_ctrl->p_gpt_u->GTADTRA = 0;               /* Delay */
        p_ctrl->p_gpt_u->GTINTAD |= 0x00010000;     /* channel B16*/

        // power on the POE circuit
        //HW_GPT_PWM_POE_PowerOn();
        // configures POE
        //HW_GPT_PWM_SetPoegA(0x30000030);

        /* Start all timer counters synchronously */  //ONLY FOR TRAPEZOIDAL MOTOR
        p_ctrl->p_gpt_v->GTSTR = (uint32_t)((1 << p_cfg->pwm_ch_u) | (1 << p_cfg->pwm_ch_v) | (1 << p_cfg->pwm_ch_w));
        p_ctrl->open = 1;


    }

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

    case COMMUTATION_TRAP:

        /* Flux orientation is based on the hall sensors, commented out for testing of sensorless motor control */
 //       p_ctrl->angle_rad = (float)(sector[p_ctrl->hall_state & 0x07] + 90) * (2.0f * M_PI / 360.0f);
//        p_ctrl->calc_beta = sinf(p_ctrl->angle_rad) * (float)p_ctrl->output_q * (1/32768.0f);
//        p_ctrl->calc_alpha = cosf(p_ctrl->angle_rad) * (float)p_ctrl->output_q * (1/32768.0f);
//        commutate_svm(p_ctrl, p_ctrl->calc_alpha, p_ctrl->calc_beta);
     //     update_pwm(p_ctrl);



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

