///*******************************************************************************
//* DISCLAIMER
//* This software is supplied by Renesas Electronics Corporation and is only
//* intended for use with Renesas products. No other uses are authorized. This
//* software is owned by Renesas Electronics Corporation and is protected under
//* all applicable laws, including copyright laws.
//* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
//* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
//* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
//* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
//* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
//* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
//* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
//* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
//* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//* Renesas reserves the right, without notice, to make changes to this software
//* and to discontinue the availability of this software. By using this software,
//* you agree to the additional terms and conditions found by accessing the
//* following link:
//* http://www.renesas.com/disclaimer
//*
//* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
//*******************************************************************************/
///*******************************************************************************
//* File Name     : m_commands.c
//* Version       : 1.00
//* Device(s)     : Synergy Family
//* Tool-Chain    : GNU GCC
//* Description   :
//******************************************************************************/
//
//#include <math.h>
//#include <r_motor_api.h>
//#include "hal_data.h"
//#include "bsp_api.h"
//
//#include "r_encoder_api.h"
//#include "sf_motion.h"
//#include "sf_motion_api.h"
//#include "motion_thread.h"
//
//#include "m_common.h"
//
//uint16_t m_state(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (!arg_set)
//        *p_arg_value = (int32_t)(cm->p_ctrl->act_state);
//    return 0;
//}
//
//uint16_t m_pos_error(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (!arg_set)
//        *p_arg_value = cm->p_ctrl->position_err;
//    return 0;
//}
//
//uint16_t m_adc1(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set == false)
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_ctrl->adc_iu);
//    return 0;
//}
//
//uint16_t m_adc2(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set == false)
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_ctrl->adc_iv);
//    return 0;
//}
//
//uint16_t m_total_current(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set == false)
//    {
//        int16_t tu = cm->p_ctrl->p_motor_ctrl->adc_iu;
//        int16_t tv = cm->p_ctrl->p_motor_ctrl->adc_iv;
//        if (tu < 0)
//            tu = (int16_t)(-tu);
//        if (tv < 0)
//            tv = (int16_t)(-tv);
//        *p_arg_value = (int32_t)(tu + tv);
//    }
//    return 0;
//}
//uint16_t m_crnt_vel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    *p_arg_value = 0;
//    return 0;
//}
//
//uint16_t m_vel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->motion_req.velocity = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->motion_req.velocity;
//    return 0;
//}
//
//uint16_t m_acc(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->motion_req.acceleration = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->motion_req.acceleration;
//    return 0;
//}
//
//uint16_t m_dec(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->motion_req.deceleration = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->motion_req.deceleration;
//    return 0;
//}
//
//uint16_t m_ajerk(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->motion_req.accel_jerk = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->motion_req.accel_jerk;
//    return 0;
//}
//
//uint16_t m_djerk(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->motion_req.decel_jerk = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->motion_req.decel_jerk;
//    return 0;
//}
//
//uint16_t m_profile(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_cfg->profile_mode = *p_arg_value;
//    else
//        *p_arg_value = cm->p_cfg->profile_mode;
//    return 0;
//}
//
//uint16_t m_kp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.kp = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.kp;
//    return 0;
//}
//
//uint16_t m_ki(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.ki = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.ki;
//    return 0;
//}
//
//uint16_t m_kd(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.kd = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.kd;
//    return 0;
//}
//
//uint16_t m_il(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.il = *p_arg_value << 8;
//    else
//        *p_arg_value = cm->p_ctrl->pos_loop_req.il >> 8;
//    return 0;
//}
//
//uint16_t m_vff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.kvff = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.kvff;
//    return 0;
//}
//
//uint16_t m_aff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.kaff = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.kaff;
//    return 0;
//}
//
//uint16_t m_ds(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->pos_loop_req.ds_time_ts = (int16_t)*p_arg_value;
//        cm->p_ctrl->pos_loop_req.ds_time_us = (int16_t)(cm->p_ctrl->pos_loop_req.ds_time_ts * 50); /* Replace the constant 50 with PWM Period */
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.ds_time_ts;
//    }
//    return 0;
//}
//
//uint16_t m_bias(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.bias = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.bias;
//    return 0;
//}
//
//uint16_t m_mlimit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.out_limit = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.out_limit;
//    return 0;
//}
//
//uint16_t m_max_error(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.pos_err_limit = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.pos_err_limit;
//    return 0;
//}
//
//uint16_t m_err_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->pos_loop_req.pos_err_time_us = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->pos_loop_req.pos_err_time_us;
//    return 0;
//}
//
//uint16_t m_astop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_cfg->auto_stop_mode = *p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_cfg->auto_stop_mode;
//    return 0;
//}
//
//uint16_t m_phasing_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_cfg->phasing_pars.mode = *p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_cfg->phasing_pars.mode;
//    return 0;
//}
//
//uint16_t m_phasing_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_cfg->phasing_pars.time = *p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_cfg->phasing_pars.time;
//    return 0;
//}
//
//uint16_t m_phasing_power(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_cfg->phasing_pars.power = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_cfg->phasing_pars.power;
//    return 0;
//}
//
//uint16_t m_phase_config(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->phase_mapping = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->phase_mapping;
//    ;
//    return 0;
//}
//
//uint16_t m_phase_origin(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_origin = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->phase_origin;
//    return 0;
//}
//
//uint16_t m_phase_offset(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_offset = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->phase_offset;
//    return 0;
//}
//
//uint16_t m_phase_angle(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_angle = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->phase_angle;
//    return 0;
//}
//
//uint16_t m_phase_adv_gain(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_adv_gain = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->phase_adv_gain;
//    return 0;
//}
//
//uint16_t m_phase_vel_comp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_vel_comp = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->phase_vel_comp;
//    return 0;
//}
//
//uint16_t m_phase_sector(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->phase_sector = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_ctrl->phase_sector;
//    return 0;
//}
//
//uint16_t m_motor_type(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->type = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->type;
//    return 0;
//}
//
//uint16_t m_commutation_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_motor_cfg->commutation_mode = (uint16_t)*p_arg_value;
//        cm->p_ctrl->p_motor_ctrl->commutation_mode = (uint16_t)*p_arg_value;
//    }
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->commutation_mode;
//    return 0;
//}
//
//uint16_t m_freq(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->pwm_freq_KHz = (uint32_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_cfg->pwm_freq_KHz);
//    return 0;
//}
//
//uint16_t m_pole_pairs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->pole_pairs = (uint8_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->pole_pairs;
//    return 0;
//}
//
//uint16_t m_ec_per_ecycle(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->ec_per_ecycle = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_ctrl->ec_per_ecycle;
//    return 0;
//}
//
//uint16_t m_tc_limit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->crnt_limit_mA = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->crnt_limit_mA;
//    return 0;
//}
//
//uint16_t m_tc_limit_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->crnt_limit_time_ms = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->crnt_limit_time_ms;
//    return 0;
//}
//
//uint16_t m_pc_limit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    if (arg_set)
//        ;
//    else
//        *p_arg_value = 0;
//    return 0;
//}
//
//uint16_t m_pc_limit_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    if (arg_set)
//        ;
//    else
//        *p_arg_value = 0;
//    return 0;
//}
//
//uint16_t m_foc_id(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_info->current_d = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_info->current_d;
//    return 0;
//}
//
//uint16_t m_foc_iq(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_info->current_q = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_info->current_q;
//    return 0;
//}
//
//uint16_t m_foc_iq_err(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_info->current_q_err = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_info->current_q_err;
//    return 0;
//}
//
//uint16_t m_foc_id_err(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_info->current_d_err = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_info->current_d_err;
//    return 0;
//}
//
//uint16_t m_foc_kp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->kp_q = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->kp_q;
//    return 0;
//}
//
//uint16_t m_foc_ki(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->ki_q = (uint16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_cfg->ki_q;
//    return 0;
//}
//
//uint16_t m_foc_qgain(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_cfg->feedback_gain = *p_arg_value;
//    else
//        *p_arg_value = cm->p_ctrl->p_motor_cfg->feedback_gain;
//    return 0;
//}
//
//uint16_t m_outputIQ(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)   /* IQ Set point */
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->output_q = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_ctrl->output_q;
//   return 0;
//}
//
//uint16_t m_outputID(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)   /* ID Set point */
//{
//    if (arg_set)
//        cm->p_ctrl->p_motor_ctrl->output_d = (int16_t)*p_arg_value;
//    else
//        *p_arg_value = (int32_t)cm->p_ctrl->p_motor_ctrl->output_d;
//    return 0;
//}
//
//uint16_t m_phase_voltage_U(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_motor_ctrl->out_u = ((float)*p_arg_value / 200.0f);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_ctrl->out_u * 200.0f);
//    }
//    return 0;
//}
//
//uint16_t m_phase_voltage_V(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_motor_ctrl->out_v = ((float)*p_arg_value / 200.0f);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_ctrl->out_v * 200.0f);
//    }
//    return 0;
//}
//
//uint16_t m_phase_voltage_W(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_motor_ctrl->out_w = ((float)*p_arg_value / 200.0f);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)(cm->p_ctrl->p_motor_ctrl->out_w * 200.0f);
//    }
//    return 0;
//}
//
//uint16_t m_encoderType(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    if (!arg_set)
//        *p_arg_value = 0;
//    return 0;
//}
//
//uint16_t m_pos_capture_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//uint16_t m_pos_hallchange(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//        ;
//    else
//        *p_arg_value = cm->p_ctrl->hall_change_pos;
//    return 0;
//}
//
//uint16_t m_halls_invert(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        uint16_t    _u, _v, _w;
//        cm->p_ctrl->p_cfg->halls_invert = (int16_t)*p_arg_value;
//        switch (*p_arg_value)
//        {
//            case 0: _u = 1; _v = 2; _w = 4; break;
//            case 1: _u = 2; _v = 1; _w = 4; break;
//            case 2: _u = 1; _v = 4; _w = 2; break;
//            case 3: _u = 4; _v = 1; _w = 2; break;
//            case 4: _u = 2; _v = 4; _w = 1; break;
//            default: _u = 4; _v = 2; _w = 1; break;
//        }
//        cm->p_ctrl->hall_bit_u = _u;
//        cm->p_ctrl->hall_bit_v = _v;
//        cm->p_ctrl->hall_bit_w = _w;
//    }
//    else
//        *p_arg_value = cm->p_ctrl->p_cfg->halls_invert;
//    return 0;
//}
//
//uint16_t m_halls_shift(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_cfg->halls_shift = (int16_t)*p_arg_value;
//    }
//    else
//        *p_arg_value = cm->p_ctrl->p_cfg->halls_shift;
//    return 0;
//}
//
//uint16_t m_pos_invert(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_encoder_cfg->reversed = *p_arg_value != 0;
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->p_encoder_cfg->reversed ? 1 : 0;
//    }
//    return 0;
//}
//
//uint16_t m_ec_per_rev(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_encoder_cfg->ec_per_rev = *p_arg_value;
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->p_encoder_cfg->ec_per_rev;
//    }
//    return 0;
//}
//
//uint16_t m_abs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->motion_req.position = *p_arg_value;
//        cm->p_ctrl->position_rel_req = 0;
//        cm->p_ctrl->motion_req.profile_mode = cm->p_cfg->profile_mode;
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->motion_req.position;
//    }
//    return 0;
//}
//
//uint16_t m_rel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->position_rel_req = *p_arg_value;
//        cm->p_ctrl->motion_req.profile_mode = cm->p_cfg->profile_mode;
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->motion_req.position;
//    }
//    return 0;
//}
//
//uint16_t m_position(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_encoder_api->write(cm->p_ctrl->p_encoder_ctrl, *p_arg_value);
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->p_encoder_ctrl->crnt.position;
//    }
//    return 0;
//}
//
//uint16_t m_index(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->p_encoder_api->trigger(cm->p_ctrl->p_encoder_ctrl, (uint32_t)*p_arg_value);
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->p_encoder_ctrl->crnt.pos_capture;
//        cm->p_api->capture(cm->p_ctrl, 0);
//    }
//    return 0;
//}
//
//uint16_t m_go(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->motion_req.position = *p_arg_value;
//    }
//    return (uint16_t)(cm->p_api->start(cm->p_ctrl));
//}
//
//uint16_t m_forward(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (!arg_set)
//    {
//        cm->p_ctrl->motion_req.profile_mode = VPGM_VelContouring;
//        cm->p_ctrl->motion_req.dir_sign = 1;
//    }
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//uint16_t m_reverse(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (!arg_set)
//    {
//        cm->p_ctrl->motion_req.profile_mode = VPGM_VelContouring;
//        cm->p_ctrl->motion_req.dir_sign = -1;
//    }
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//uint16_t m_powerOn(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return (uint16_t)(cm->p_api->power(cm->p_ctrl, true));
//}
//
//uint16_t m_powerOff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return (uint16_t)(cm->p_api->power(cm->p_ctrl, false));
//}
//
//uint16_t m_servoOn(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return (uint16_t)(cm->p_api->servo(cm->p_ctrl, true));
//}
//
//uint16_t m_servoOff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return (uint16_t)(cm->p_api->servo(cm->p_ctrl, false));
//}
//
//uint16_t m_smoothStop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    cm->p_api->stop(cm->p_ctrl, MOTION_STOP_SMOOTH);
//    return 0;
//}
//
//uint16_t m_abruptStop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    cm->p_api->stop(cm->p_ctrl, MOTION_STOP_ABRUPT);
//    return 0;
//}
//
//uint16_t m_alignPhase(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    sf_phasing_mode_t align_mode = cm->p_cfg->phasing_pars.mode;
//
//    if (arg_set)
//    {
//        align_mode = (sf_phasing_mode_t)*p_arg_value;
//    }
//    return (uint16_t)(cm->p_api->align(cm->p_ctrl, align_mode));
//}
//
//
//uint16_t m_inputs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    if (!arg_set)
//    {
//        int32_t tmp;
//        tmp  = (int32_t)(g_sf_motion1.p_ctrl->hall_state & 0x000f);
//        tmp |= (int32_t)(g_sf_motion2.p_ctrl->hall_state & 0x000f) << 4;
//
//        *p_arg_value = (int32_t)(uint32_t)(tmp);// + (fault ? 0x0000 : 0) + tmp;
//    }
//    return 0;
//}
//
//uint16_t m_auto_brake(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->auto_brake = *p_arg_value ? true : false;
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->auto_brake;
//    }
//    return 0;
//}
//
//uint16_t m_reset(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//uint16_t m_version(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    *p_arg_value = (int32_t)"S7G2 v.1.0 Biplane, (c) Renesas Electronics America";
//    return 0;
//}
//
//uint16_t m_save(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//uint16_t m_restore(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//    SSP_PARAMETER_NOT_USED(p_arg_value);
//    return 0;
//}
//
//
//uint16_t m_trace_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->trace.mode = (int16_t)*p_arg_value;
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.mode;
//    }
//    return 0;
//}
//
//uint16_t m_trace_rate_mult(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->trace.rateMult = (int16_t)*p_arg_value;
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.rateMult;
//    }
//    return 0;
//}
//
//uint16_t m_trace_level(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_ctrl->trace.level = (int16_t)*p_arg_value;
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.level;
//    }
//    return 0;
//}
//
//uint16_t m_logChannel0(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_api->logging(cm->p_ctrl, *p_arg_value, 0);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.code[0];
//    }
//    return 0;
//}
//
//uint16_t m_logChannel1(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_api->logging(cm->p_ctrl, *p_arg_value, 1);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.code[1];
//    }
//    return 0;
//}
//
//uint16_t m_logChannel2(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_api->logging(cm->p_ctrl, *p_arg_value, 2);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.code[2];
//    }
//    return 0;
//}
//
//uint16_t m_logChannel3(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    if (arg_set)
//    {
//        cm->p_api->logging(cm->p_ctrl, *p_arg_value, 3);
//    }
//    else
//    {
//        *p_arg_value = (int32_t)cm->p_ctrl->trace.code[3];
//    }
//    return 0;
//}
//
//uint16_t m_trace(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    // Configure data tracing
//    if (arg_set)
//    {
//        cm->p_ctrl->trace.trigger = (int16_t)*p_arg_value;
//        if (!cm->p_ctrl->trace.trigger)
//        {
//            cm->p_ctrl->trace.mode = 0; // Stop data recording
//            return 0;
//        }
//        cm->p_ctrl->trace.skip = 0;
//        cm->p_ctrl->trace.index = 0;
//        cm->p_ctrl->trace.count = 0;
//        switch (cm->p_ctrl->trace.trigger)
//        {
//        case 1:     // Start now continuous recording
//            cm->p_ctrl->trace.mode = 3;
//            break;
//        case 2:     // Start now, Stop on buffer full
//            cm->p_ctrl->trace.mode = 2;
//            break;
//        case 3:     // Start on begin of motion, Stop on end of motion
//            cm->p_ctrl->trace.mode = 1; // Armed
//            break;
//        case 4:     // Start now, Stop on input change (threshold is input mask)
//            //inputs_save = (unsigned long)m1.dinputs | ((unsigned long)m2.dinputs << 16);
//            cm->p_ctrl->trace.mode = 3;
//            break;
//        case 5:     // Start now, Stop on value0 > threshold
//        case 6:     // Start now, Stop on value0 < threshold
//            cm->p_ctrl->trace.mode = 3;
//            break;
//        case 7:     // Start on begin of motion (when servo is on) or PWM output change (when servo is off)
//            cm->p_ctrl->trace.mode = 1; // Armed
//            break;
//        case 8:     // Start on input change (threshold is input mask)
//            //inputs_save = (unsigned long)m1.dinputs | ((unsigned long)m2.dinputs << 16);
//            cm->p_ctrl->trace.mode = 1; // Armed
//            break;
//        case 9:     // Start on value0 > threshold, Stop on buffer full
//        case 10:    // Start on value0 < threshold, Stop on buffer full
//            cm->p_ctrl->trace.mode = 1; // Armed
//            break;
//        }
//    }
//    else
//    {
//        *p_arg_value = cm->p_ctrl->trace.trigger;
//    }
//    return 0;
//}
//
////#include "sf_uart_comms.h"
//extern sf_comms_instance_t const * p_comms;
//
//uint16_t m_play(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value)
//{
//    SSP_PARAMETER_NOT_USED(cm);
//    SSP_PARAMETER_NOT_USED(arg_set);
//
//    int32_t idx;
//    int16_t save;
//    m_console_t *pc = (m_console_t *)p_arg_value;
//
//    save = cm->p_ctrl->trace.mode;
//    cm->p_ctrl->trace.mode = 0;
//    // Display g_trace buffers
//    if (cm->p_ctrl->trace.count < TRACE_BUFFER_SIZE)
//        cm->p_ctrl->trace.index = 0;
//    for (idx = 0; idx < cm->p_ctrl->trace.count; idx++)
//    {
//        // Print idx, data0, data1, data2, CR/LF
//        if (cm->p_ctrl->trace.code[0] < 8) {
//            Long2StrDec (pc, (int32_t)(((int32_t)cm->p_ctrl->trace.data[cm->p_ctrl->trace.index + TRACE_BUFFER_SIZE])<<16) |
//                                (int32_t)(uint32_t)cm->p_ctrl->trace.data[cm->p_ctrl->trace.index]);
//        } else {
//            Long2StrDec (pc, (int32_t)(int16_t)cm->p_ctrl->trace.data[cm->p_ctrl->trace.index]);
//        }
//        *(pc->ptx++) = ',';
//        Long2StrDec (pc, (int16_t)cm->p_ctrl->trace.data[cm->p_ctrl->trace.index + TRACE_BUFFER_SIZE*2]);
//        *(pc->ptx++) = ',';
//
//        Long2StrDec (pc, (int16_t)cm->p_ctrl->trace.data[cm->p_ctrl->trace.index + TRACE_BUFFER_SIZE*3]);
//        *(pc->ptx++) = 13;
//        *(pc->ptx++) = 10;
//        pc->tx_count += 4;
//
//        p_comms->p_api->write(p_comms->p_ctrl, pc->tx_buf, pc->tx_count, TX_WAIT_FOREVER);
//
//        pc->ptx = pc->tx_buf;
//        pc->tx_count = 0;
//
//        cm->p_ctrl->trace.index++;
//        if (cm->p_ctrl->trace.index >= TRACE_BUFFER_SIZE)
//            cm->p_ctrl->trace.index = 0;
//    }
//    cm->p_ctrl->trace.mode = save;
//    return 0;
//}
//
