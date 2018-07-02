/* generated thread source file - do not edit */
#include "motion_thread.h"

TX_THREAD motion_thread;
void motion_thread_create(void);
static void motion_thread_func(ULONG thread_input);
/** Alignment requires using pragma for IAR. GCC is done through attribute. */
#if defined(__ICCARM__)
#pragma data_alignment = BSP_STACK_ALIGNMENT
#endif
static uint8_t motion_thread_stack[1024] BSP_PLACE_IN_SECTION(".stack.motion_thread") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_isr, ADC, SCAN_END, 0);
#endif
#endif
#if (BSP_IRQ_DISABLED) != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_adc0) && !defined(SSP_SUPPRESS_ISR_ADC0)
SSP_VECTOR_DEFINE_CHAN(adc_scan_end_b_isr, ADC, SCAN_END_B, 0);
#endif
#endif
adc_instance_ctrl_t g_adc0_ctrl;
const adc_cfg_t g_adc0_cfg =
{ .unit = 0, .mode = ADC_MODE_SINGLE_SCAN, .resolution = ADC_RESOLUTION_8_BIT, .alignment = ADC_ALIGNMENT_RIGHT,
  .add_average_count = ADC_ADD_OFF, .clearing = ADC_CLEAR_AFTER_READ_ON, .trigger = ADC_TRIGGER_SOFTWARE,
  .trigger_group_b = ADC_TRIGGER_SYNC_ELC, .p_callback = NULL, .p_context = &g_adc0, .scan_end_ipl = (BSP_IRQ_DISABLED),
  .scan_end_b_ipl = (BSP_IRQ_DISABLED), };
const adc_channel_cfg_t g_adc0_channel_cfg =
{ .scan_mask = (uint32_t) (
        ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | (0)),
  /** Group B channel mask is right shifted by 32 at the end to form the proper mask */
  .scan_mask_group_b = (uint32_t) (
          (((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0)
                  | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | ((uint64_t) 0) | (0)) >> 32),
  .priority_group_a = ADC_GROUP_A_PRIORITY_OFF, .add_mask = (uint32_t) (
          (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)
                  | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0) | (0)),
  .sample_hold_mask = (uint32_t) ((0) | (0) | (0)), .sample_hold_states = 24, };
/* Instance structure to use this module. */
const adc_instance_t g_adc0 =
{ .p_ctrl = &g_adc0_ctrl, .p_cfg = &g_adc0_cfg, .p_channel_cfg = &g_adc0_channel_cfg, .p_api = &g_adc_on_adc };
/************* ENCODER on GPT ****************/
encoder_ctrl_t g_r_encoder_gpt2_ctrl;
encoder_cfg_t g_r_encoder_gpt2_cfg =
{ .channel = 8, .channel_aux = 0, .ec_per_rev = 4000, .reversed = 0, .trigger = 1, .p_extend = NULL, };

encoder_instance_t g_r_encoder_gpt2 =
{ .p_ctrl = &g_r_encoder_gpt2_ctrl, .p_cfg = &g_r_encoder_gpt2_cfg, .p_api = &g_encoder_on_gpt };
/** BLDC Motor on Motor instance */
#if SYNERGY_NOT_DEFINED != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_r_motor_bldc2) && !defined(SSP_SUPPRESS_ISR_GPT0)
void pwm_counter_overflow (void);
#define SSP_SUPPRESS_ISR_GPT0
SSP_VECTOR_DEFINE_CHAN(pwm_counter_overflow, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif

motor_ctrl_t g_r_motor_bldc2_ctrl;
motor_cfg_t g_r_motor_bldc2_cfg =
{
/* Motor parameters */
.type = MOTOR_BLDC,
  .pole_pairs = 4, .inductance_uH = 1200, .bemf_v_krpm = 15, .bus_voltage = 24,

  /* PWM Generator configuration */
  .pwm_ch_u = 0,
  .pwm_ch_v = 1, .pwm_ch_w = 2,

  /* Control algorithm configuration */
  .commutation_mode = COMMUTATION_SIN,
  .control_mode = CONTROL_MODE_VOLTAGE,

  .dead_time_us = 1,
  .pwm_freq_KHz = 60, .output_mode = PWM_OUT_INDEPENDENT_A,

  /* Current feedback configuration */
  .adc_ch_u = 0,
  .adc_ch_v = 1, .adc_range_mA = 15000,

  .crnt_limit_mA = 6000,
  .crnt_limit_time_ms = 100,

  /* FOC algorithm configuration */
  .kp_d = 100,
  .ki_d = 100, .kp_q = 100, .ki_q = 100,

  .p_extend = NULL, };

motor_instance_t g_r_motor_bldc2 =
{ .p_ctrl = &g_r_motor_bldc2_ctrl, .p_cfg = &g_r_motor_bldc2_cfg, .p_api = &g_motor_bldc };
/************* MOTION CONTROL FRAMEWORK ****************/
#if MOTION_CALLBACK_USED_g_sf_motion2
#if defined(__ICCARM__)
#define NULL_WEAK_ATTRIBUTE
#pragma weak NULL = NULL_internal
#elif defined(__GNUC__)
#define NULL_WEAK_ATTRIBUTE __attribute__ ((weak, alias("NULL_internal")))
#endif
void NULL(sf_motion_callback_args_t * p_args) NULL_WEAK_ATTRIBUTE;
#endif

sf_motion_ctrl_t g_sf_motion2_ctrl;
sf_motion_cfg_t g_sf_motion2_cfg =
{
/* Requested Modes of operation */
.profile_mode = VPGM_Trapezoidal,
  .auto_stop_mode = MOTION_STOP_SMOOTH, .pos_loop_dflt =
  { .kp = 20, .ki = 20, .kd = 500, .il = 300 << 8, .ds_time_us = 100, },
  .phasing_pars =
  { .mode = PIM_FORCED, .time = 500, .power = 3000, },

  /* Amplifier interface configuration */
  .pin_ae = IOPORT_PORT_03_PIN_05,
  .pin_fault = IOPORT_PORT_06_PIN_06,

  /* Hall sensor feedback configuration */
  .pin_hall_u = 0,
  .pin_hall_v = 0, .pin_hall_w = 0,

  /* Brake output configuration */
  .pin_brake = 0,

  /* Fault control parameters */
  .pos_err_limit_ec = 1000,
  .pos_err_time_ms = 100,

  .p_encoder = &g_r_encoder_gpt2,
  .p_motor = &g_r_motor_bldc2, .p_callback = NULL, .p_extend = NULL, };

sf_motion_instance_t g_sf_motion2 =
{ .p_ctrl = &g_sf_motion2_ctrl, .p_cfg = &g_sf_motion2_cfg, .p_api = &g_sf_motion_on_sf_motion, };

#if MOTION_CALLBACK_USED_g_sf_motion2
/*******************************************************************************************************************//**
 * @brief      This is a weak example callback function.  It should be overridden by defining a user callback function 
 *             with the prototype below.
 *               - void NULL(sf_motion_callback_args_t * p_args)
 *
 * @param[in]  p_args  Callback arguments used to identify what caused the callback.
 **********************************************************************************************************************/
void NULL_internal(sf_motion_callback_args_t * p_args);
void NULL_internal(sf_motion_callback_args_t * p_args)
{
    /** Do nothing. */
    SSP_PARAMETER_NOT_USED(p_args);
}
#endif
/************* ENCODER on GPT ****************/
encoder_ctrl_t g_r_encoder_gpt1_ctrl;
encoder_cfg_t g_r_encoder_gpt1_cfg =
{ .channel = 8, .channel_aux = 0, .ec_per_rev = 4000, .reversed = 0, .trigger = 1, .p_extend = NULL, };

encoder_instance_t g_r_encoder_gpt1 =
{ .p_ctrl = &g_r_encoder_gpt1_ctrl, .p_cfg = &g_r_encoder_gpt1_cfg, .p_api = &g_encoder_on_gpt };
/** BLDC Motor on Motor instance */
#if SYNERGY_NOT_DEFINED != BSP_IRQ_DISABLED
#if !defined(SSP_SUPPRESS_ISR_g_r_motor_bldc1) && !defined(SSP_SUPPRESS_ISR_GPT0)
void pwm_counter_overflow (void);
#define SSP_SUPPRESS_ISR_GPT0
SSP_VECTOR_DEFINE_CHAN(pwm_counter_overflow, GPT, COUNTER_OVERFLOW, 0);
#endif
#endif

motor_ctrl_t g_r_motor_bldc1_ctrl;
motor_cfg_t g_r_motor_bldc1_cfg =
{
/* Motor parameters */
.type = MOTOR_BLDC,
  .pole_pairs = 2, .inductance_uH = 1200, .bemf_v_krpm = 15, .bus_voltage = 24,

  /* PWM Generator configuration */
  .pwm_ch_u = 0,
  .pwm_ch_v = 1, .pwm_ch_w = 2,

  /* Control algorithm configuration */
  .commutation_mode = COMMUTATION_TRAP,
  .control_mode = CONTROL_MODE_VOLTAGE,

  .dead_time_us = 1,
  .pwm_freq_KHz = 60, .output_mode = PWM_OUT_COMPLEMENTARY,

  /* Current feedback configuration */
  .adc_ch_u = 0,
  .adc_ch_v = 1, .adc_range_mA = 15000,

  .crnt_limit_mA = 6000,
  .crnt_limit_time_ms = 100,

  /* FOC algorithm configuration */
  .kp_d = 100,
  .ki_d = 100, .kp_q = 100, .ki_q = 100,

  .p_extend = NULL, };

motor_instance_t g_r_motor_bldc1 =
{ .p_ctrl = &g_r_motor_bldc1_ctrl, .p_cfg = &g_r_motor_bldc1_cfg, .p_api = &g_motor_bldc };
/************* MOTION CONTROL FRAMEWORK ****************/
#if MOTION_CALLBACK_USED_g_sf_motion1
#if defined(__ICCARM__)
#define NULL_WEAK_ATTRIBUTE
#pragma weak NULL = NULL_internal
#elif defined(__GNUC__)
#define NULL_WEAK_ATTRIBUTE __attribute__ ((weak, alias("NULL_internal")))
#endif
void NULL(sf_motion_callback_args_t * p_args) NULL_WEAK_ATTRIBUTE;
#endif

sf_motion_ctrl_t g_sf_motion1_ctrl;
sf_motion_cfg_t g_sf_motion1_cfg =
{
/* Requested Modes of operation */
.profile_mode = VPGM_Trapezoidal,
  .auto_stop_mode = MOTION_STOP_SMOOTH, .pos_loop_dflt =
  { .kp = 20, .ki = 20, .kd = 500, .il = 300 << 8, .ds_time_us = 100, },
  .phasing_pars =
  { .mode = PIM_FORCED, .time = 500, .power = 3000, },

  /* Amplifier interface configuration */
  .pin_ae = IOPORT_PORT_03_PIN_05,
  .pin_fault = IOPORT_PORT_06_PIN_06,

  /* Hall sensor feedback configuration */
  .pin_hall_u = 0,
  .pin_hall_v = 0, .pin_hall_w = 0,

  /* Brake output configuration */
  .pin_brake = 0,

  /* Fault control parameters */
  .pos_err_limit_ec = 1000,
  .pos_err_time_ms = 100,

  .p_encoder = &g_r_encoder_gpt1,
  .p_motor = &g_r_motor_bldc1, .p_callback = NULL, .p_extend = NULL, };

sf_motion_instance_t g_sf_motion1 =
{ .p_ctrl = &g_sf_motion1_ctrl, .p_cfg = &g_sf_motion1_cfg, .p_api = &g_sf_motion_on_sf_motion, };

#if MOTION_CALLBACK_USED_g_sf_motion1
/*******************************************************************************************************************//**
 * @brief      This is a weak example callback function.  It should be overridden by defining a user callback function 
 *             with the prototype below.
 *               - void NULL(sf_motion_callback_args_t * p_args)
 *
 * @param[in]  p_args  Callback arguments used to identify what caused the callback.
 **********************************************************************************************************************/
void NULL_internal(sf_motion_callback_args_t * p_args);
void NULL_internal(sf_motion_callback_args_t * p_args)
{
    /** Do nothing. */
    SSP_PARAMETER_NOT_USED(p_args);
}
#endif
extern bool g_ssp_common_initialized;
extern uint32_t g_ssp_common_thread_count;
extern TX_SEMAPHORE g_ssp_common_initialized_semaphore;
void g_hal_init(void);

void motion_thread_create(void)
{
    /* Increment count so we will know the number of ISDE created threads. */
    g_ssp_common_thread_count++;

    /* Initialize each kernel object. */

    tx_thread_create (&motion_thread, (CHAR *) "Motion Thread", motion_thread_func, (ULONG) NULL, &motion_thread_stack,
                      1024, 7, 7, 1, TX_AUTO_START);
}

static void motion_thread_func(ULONG thread_input)
{
    /* Not currently using thread_input. */
    SSP_PARAMETER_NOT_USED (thread_input);

    /* First thread will take care of common initialization. */
    UINT err;
    err = tx_semaphore_get (&g_ssp_common_initialized_semaphore, TX_WAIT_FOREVER);

    while (TX_SUCCESS != err)
    {
        /* Check err, problem occurred. */
        BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
    }

    /* Only perform common initialization if this is the first thread to execute. */
    if (false == g_ssp_common_initialized)
    {
        /* Later threads will not run this code. */
        g_ssp_common_initialized = true;

        /* Perform common module initialization. */
        g_hal_init ();

        /* Now that common initialization is done, let other threads through. */
        /* First decrement by 1 since 1 thread has already come through. */
        g_ssp_common_thread_count--;
        while (g_ssp_common_thread_count > 0)
        {
            err = tx_semaphore_put (&g_ssp_common_initialized_semaphore);

            while (TX_SUCCESS != err)
            {
                /* Check err, problem occurred. */
                BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);
            }

            g_ssp_common_thread_count--;
        }
    }

    /* Initialize each module instance. */
    /************* MOTION CONTROL FRAMEWORK ****************/
    ssp_err_t ssp_err_g_sf_motion2;
    ssp_err_g_sf_motion2 = g_sf_motion2.p_api->open (g_sf_motion2.p_ctrl, g_sf_motion2.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_motion2)
    {
        while (1)
            ;
    }
    /************* MOTION CONTROL FRAMEWORK ****************/
    ssp_err_t ssp_err_g_sf_motion1;
    ssp_err_g_sf_motion1 = g_sf_motion1.p_api->open (g_sf_motion1.p_ctrl, g_sf_motion1.p_cfg);
    if (SSP_SUCCESS != ssp_err_g_sf_motion1)
    {
        while (1)
            ;
    }

    /* Enter user code for this thread. */
    motion_thread_entry ();
}
