/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name     : m_common.h
* Version       : 1.00
* Device(s)     : Synergy Family
* Tool-Chain    : GNU GCC
* Description   :
******************************************************************************/

#include "sf_motion.h"

/* Buffers size definitions */
#define TX_BUF_SIZE     100
#define RX_BUF_SIZE     50

#define PP_BUFF_SIZE    100
#define PC_BUFF_SIZE    10
#define VPG_BUFF_SIZE   20
#define TRACE_BUFFER_SIZE 512

#define SW1 (IOPORT_PORT_02_PIN_07)
#define SW2 (IOPORT_PORT_02_PIN_04)
#define AEN1 (IOPORT_PORT_03_PIN_05)
#define AEN2 (IOPORT_PORT_03_PIN_06)
#define ETH0_RST (IOPORT_PORT_01_PIN_13)
#define ETH1_RST (IOPORT_PORT_01_PIN_14)
#define INP1 (IOPORT_PORT_06_PIN_08)
#define INP2 (IOPORT_PORT_06_PIN_09)
#define INP3 (IOPORT_PORT_06_PIN_10)
#define INP4 (IOPORT_PORT_06_PIN_11)
#define INP5 (IOPORT_PORT_06_PIN_12)
#define INP6 (IOPORT_PORT_06_PIN_13)
#define INP7 (IOPORT_PORT_06_PIN_14)
#define INP8 (IOPORT_PORT_06_PIN_15)
#define TX2EN (IOPORT_PORT_10_PIN_08)
#define EZ2EN (IOPORT_PORT_10_PIN_09)
#define EZ1EN (IOPORT_PORT_10_PIN_10)
#define BRAKE1 (IOPORT_PORT_10_PIN_00)
#define BRAKE2 (IOPORT_PORT_10_PIN_01)
#define FAULT1 (IOPORT_PORT_06_PIN_06)
#define FAULT2 (IOPORT_PORT_06_PIN_07)
#define HALL_W2 (IOPORT_PORT_06_PIN_05)
#define HALL_V2 (IOPORT_PORT_06_PIN_04)
#define HALL_U2 (IOPORT_PORT_06_PIN_03)
#define HALL_W1 (IOPORT_PORT_06_PIN_02)
#define HALL_V1 (IOPORT_PORT_06_PIN_01)
#define HALL_U1 (IOPORT_PORT_06_PIN_00)
#define CFG0 (IOPORT_PORT_08_PIN_00)
#define CFG1 (IOPORT_PORT_08_PIN_01)
#define CFG2 (IOPORT_PORT_08_PIN_02)
#define CFG3 (IOPORT_PORT_08_PIN_03)
#define CFG4 (IOPORT_PORT_08_PIN_04)
#define CFG5 (IOPORT_PORT_08_PIN_05)
#define CFG6 (IOPORT_PORT_08_PIN_06)
#define CFG7 (IOPORT_PORT_0B_PIN_00)
#define RS232SEL (IOPORT_PORT_05_PIN_13)
#define IN_F1 (IOPORT_PORT_09_PIN_00)
#define IN_R1 (IOPORT_PORT_09_PIN_01)
#define IN_X2 (IOPORT_PORT_09_PIN_08)
#define IN_R2 (IOPORT_PORT_09_PIN_07)
#define IN_F2 (IOPORT_PORT_09_PIN_06)
#define IN_X1 (IOPORT_PORT_09_PIN_05)

#define ENC1_ON_LED (IOPORT_PORT_03_PIN_08)
#define ENC1_ERR_LED (IOPORT_PORT_03_PIN_09)
#define ENC2_ON_LED (IOPORT_PORT_03_PIN_10)
#define ENC2_ERR_LED (IOPORT_PORT_03_PIN_11)
#define FAULT1_LED (IOPORT_PORT_03_PIN_12)
#define FAULT2_LED (IOPORT_PORT_03_PIN_13)
#define RUN_LED (IOPORT_PORT_03_PIN_14)
#define RS232_LED (IOPORT_PORT_03_PIN_15)


typedef struct
{
    uint8_t tx_buf[TX_BUF_SIZE];
    uint8_t rx_buf[RX_BUF_SIZE];
    uint8_t *ptx, *prx;
    uint32_t volatile rx_count, tx_count;
    int32_t arg;
    uint16_t err;
    uint8_t set;
    uint8_t channel;
} m_console_t;


short m_interpreter(m_console_t *pc, sf_motion_instance_t *p_motor);
void Long2StrDec (m_console_t *pc, long value);

uint16_t m_state(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pos_error(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_adc1(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_adc2(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_total_current(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_crnt_vel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_vel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_acc(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_dec(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_ajerk(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_djerk(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_profile(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_kp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_ki(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_kd(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_il(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_vff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_aff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_bp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_max_error(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_err_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_ds(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_mlimit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_bias(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_astop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_phasing_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phasing_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phasing_power(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_config(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_origin(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_motor_type(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_commutation_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_sector(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pole_pairs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_ec_per_ecycle(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_ec_per_rev(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_offset(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_angle(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_adv_gain(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_vel_comp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_pc_limit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pc_limit_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_tc_limit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_tc_limit_time(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_foc_id(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_iq(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_iq_err(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_id_err(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_kp(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_ki(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_foc_qgain(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_pos_capture_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pos_hallchange(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_halls_invert(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_halls_shift(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_trace_mode(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_trace_rate_mult(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_trace_level(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_abs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_rel(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_position(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_inputs(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_auto_brake(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_index(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_go(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_forward(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_reverse(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_reset(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_servoOn(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_servoOff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_powerOn(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_powerOff(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_smoothStop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_abruptStop(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_alignPhase(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_version(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_outputIQ(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_outputIQ(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_outputID(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_voltage_U(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_voltage_V(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_phase_voltage_W(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_encoderType(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);

uint16_t m_logChannel0(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_logChannel1(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_logChannel2(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_logChannel3(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_trace(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_play(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pLimit(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pLimitTime(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_pos_invert(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_save(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_restore(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
uint16_t m_freq(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
