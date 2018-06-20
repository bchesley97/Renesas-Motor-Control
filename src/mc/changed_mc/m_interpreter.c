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
//* File Name     : m_interpreter.c
//* Version       : 1.00
//* Device(s)     : Synergy Family
//* Tool-Chain    : GNU GCC
//* Description   :
//******************************************************************************/
//
//
//#include <stdlib.h>
//#include <ctype.h>
//#include <r_motor_api.h>
//
//#include "r_encoder_api.h"
//#include "sf_motion.h"
//
//#include "m_common.h"
//
//char sPCT[]   = "PCT";          // Pos Capture Trigger
//char sSETUP[] = "SETUP";        // Setup PMAP and HINVERT after alignment of the rotor
//char sSIM[]   = "SIM";          // Enable Simulation mode
//char sPHASES[]= "PHASES";       // Phase count
//char sHOFFS[] = "HOFFS";        // Home Offset
//char sTYPE[]  = "TYPE";         //
//char sETYPE[] = "ETYPE";        // Encoder type: 0 - incremental, 1 - absolute
//char sHTYPE[] = "HTYPE";        // Hall sensors type: 0 - parallel, 1 - serial
//char sFREQ[]  = "FREQ";         // PWM Carrier frequency
//char sSAVE[]  = "SAVE";         // Save parameters to Data Flash
//char sREST[]  = "RESTORE";      // Restore parameters from Data Flash
//char sHSCAN[] = "HSCAN";        // Halls Scanning
//char sASTOP[] = "ASTOP";        // Auto stop mode
//char sADC1[]  = "ADC1";         // Read ADC 1 value
//char sADC2[]  = "ADC2";         // Read ADC 2 value
//char sTC[]    = "TC";           // Total Current
//char sABRAKE[]= "AUTOBRAKE";    // Auto brake control
//
//char sIDM[]   = "IDM";          // D Current Measured
//char sIQM[]   = "IQM";          // Q Current Measured
//char sIQERR[] = "IQERR";        // Q Current Error
//char sIDERR[] = "IDERR";        // D Current Error
//char sIQCMD[] = "IQCMD";        // IQ Desired (PID Output)
//char sIDCMD[] = "IDCMD";        // ID Desired
//
//char sGO[]       = "GO";        // Update motion and PID parameters
//char sALIGN[]    = "ALIGN";     // Start phase alignment procedure
//char sRESET[]    = "RESET";     // Reset
//char sSERVOON[]  = "ON";        // Servo on
//char sSERVOOFF[] = "OFF";       // Servo off
//char sENABLE[]   = "ENABLE";    // Enable Power
//char sDISABLE[]  = "DISABLE";   // Enable Power
//char sSTOP[]     = "STOP";      // Stop smoothly
//char sABORT[]    = "ABORT";     // Stop abruptly
//char sHOME[]     = "HOME";      // Home axis
//char sPVT[]      = "PVT";       // Adds ABS/VEL to the PVT buffer
//
//char sHINVERT[]  = "HINVERT";   // Halls Invert Mask
//char sHSHIFT[]   = "HSHIFT";    // Halls Shift Mask
//char sHPOS[]     = "HPOS";      // Halls Transition position
//char sPINVERT[]  = "PINVERT";   // position Invert Mask
//
//char sVER[]      = "VER";       // Firmware Version
//char sSTA[]   = "STA";          // Activity status
//char sPOS[]   = "POS";          // Actual position
//char sERR[]   = "ERR";          // position error
//char sIND[]   = "IND";          // Index position
//char sINP[]   = "INP";          // Inputs state
//
//char sOUT[]   = "OUT";          // Outputs state
//char sOUT1[]  = "OUT1";         // Outputs state
//char sOUT2[]  = "OUT2";         // Outputs state
//
//char sPRO[]   = "PRO";          // velocity profile mode
//char sABS[]   = "ABS";          // Target position
//char sREL[]   = "REL";          // Relative target position
//char sVEL[]   = "VEL";          // Target velocity
//char sACC[]   = "ACC";          // Target acceleration
//char sDEC[]   = "DEC";          // Target deceleration
//char sJERK[]  = "JERK";         // acceleration jerk
//char sAJERK[] = "AJERK";        // acceleration jerk
//char sDJERK[] = "DJERK";        // deceleration jerk
//
///* FOC Control Loop Gains */
//char sQKP[]   = "QKP";
//char sQKI[]   = "QKI";
//
///* position Loop PID Regulator Gains*/
//char sKP[]    = "KP";
//char sKD[]    = "KD";
//char sKI[]    = "KI";
//char sIL[]    = "IL";
//char sDS[]    = "DS";           // Derivative Sample time
//char sAFF[]   = "AFF";
//char sVFF[]   = "VFF";
//char sBIAS[]  = "BIAS";         // Motor bias
//
///* Limits */
//char sMLIMIT[]= "MLIMIT";       // Motor limit
//char sCLIMIT[]= "CLIMIT";       // Continuous Current limit
//char sCTIME[] = "CTIME";        // Continuous Current limit time
//char sPLIMIT[]= "PLIMIT";       // Peak current limit
//char sPTIME[] = "PTIME";        // Peak current time
//char sMAX[]   = "MAX";          // position error limit
//char sETIME[] = "ETIME";        // position error time
//char sPWM[]   = "PWM";
//char sQGAIN[] = "QGAIN";        // Current loop set-point scaling
//
//char sFWD[]   = "FWD";          // Jogging (optionally for a certain time)
//char sREV[]   = "REV";
//
//char sPCMODE[]= "PCMODE";       // Phase commutation mode
//char sPIMODE[]= "PIMODE";       // Phase init mode
//char sPITIME[]= "PITIME";       // Phase init time
//char sPIOUT[] = "PIOUT";        // Phase init output
//char sPCOUNTS[] = "PCOUNTS";    // Phase counts per electrical cycle
//char sPANGLE[]= "PANGLE";       // Phase angle
//char sPORIGIN[]="PORIGIN";      // Phase origin
//char sPPAIRS[]= "PPAIRS";       // Pole pairs
//char sECPR[]  = "ECPR";         // Encoder counts per revolution
//char sPSCALE[]= "PSCALE";       // Phase prescale
//char sPMAP[]  = "PMAP";         // Phase mapping code (to outputs)
//char sPVECTOR[]="PVECTOR";      // Phase vector
//char sPIOFFS[] ="PIOFFS";       // Phase-Index Offset
//char sCV[]    ="CV";            // Current velocity
//char sPADV[]  ="PADV";          // Phase advance
//char sVCOMP[] ="VCOMP";         // Phase velocity feed forward compensation
//char sTIMER[] ="TIMER";         // PWM Carrier Interrupt handling time in nanoseconds
//
//char sVU[] = "VU";              // Phase Voltage U
//char sVV[] = "VV";              // Phase Voltage V
//char sVW[] = "VW";              // Phase Voltage W
//
//char sECP[] = "ECP";            // Max error at Commanded position
//char sECV[] = "ECV";            // Max error at Commanded velocity
//char sEPO[] = "EPO";            // Max error at position
//
///* Data Collection Commands */
//char sCH1[]   = "CH1";          // Channel 1 Trace Source
//char sCH2[]   = "CH2";          // Channel 2 Trace Source
//char sCH3[]   = "CH3";          // Channel 3 Trace Source
//char sCH4[]   = "CH4";          // Channel 3 Trace Source
//char sPLAY[]  = "PLAY";         // Display Trace Samples
//char sTRACE[] = "TRACE";        // Trace Trigger setup
//char sTMODE[] = "TMODE";        // Trace Mode
//char sTRATE[] = "TRATE";        // Trace Rate
//char sTLEVEL[]= "TLEVEL";       // Trigger Level
//
//typedef enum CommandCode {
//   CC_RW_WORD,
//   CC_RW_LONG,
//   CC_RO_WORD,
//   CC_RO_LONG,
//   CC_RW_HEX,
//   CC_RO_HEX,
//   CC_FUNC,                     // Function with no argument
//   CC_FUNC_OA,                  // Function with optional 32bit argument
//   CC_FUNC_IO,                  // Function with non-standard I/O parameters
//   CC_RW_INT,
//   CC_RO_INT,
//   CC_RO_STR,
//   CC_INVALID
//} cmd_code_t;
//
//typedef uint16_t (*m_func_t)(sf_motion_instance_t *cm, bool arg_set, int32_t *p_arg_value);
//
//typedef struct {
//    cmd_code_t CmdCode;
//   char *pName;
//   m_func_t fptr;
//} command_t;
//
///* Command table */
//command_t Commands[] = {
//    /* Status */
//    {CC_RW_LONG, sPOS,      m_position},
//    {CC_RO_INT,  sERR,      m_pos_error},
//    {CC_RO_HEX,  sINP,      m_inputs},
//    {CC_RO_HEX,  sSTA,      m_state},
//    {CC_RO_INT,  sADC1,     m_adc1},
//    {CC_RO_INT,  sADC2,     m_adc2},
//    {CC_RO_WORD, sTC,       m_total_current},
//    {CC_RO_LONG, sCV,       m_crnt_vel},
//    /* Motion Profile Configuration parameters */
//    {CC_RW_LONG, sVEL,      m_vel},
//    {CC_RW_LONG, sACC,      m_acc},
//    {CC_RW_LONG, sDEC,      m_dec},
//    {CC_RW_LONG, sAJERK,    m_ajerk},
//    {CC_RW_LONG, sDJERK,    m_djerk},
//    {CC_RW_WORD, sPRO,      m_profile},
//    /* Position Loop Configuration parameters */
//    {CC_RW_WORD, sKP,       m_kp},
//    {CC_RW_WORD, sKI,       m_ki},
//    {CC_RW_WORD, sKD,       m_kd},
//    {CC_RW_WORD, sIL,       m_il},
//    {CC_RW_WORD, sVFF,      m_vff},
//    {CC_RW_WORD, sAFF,      m_aff},
//    {CC_RW_WORD, sMAX,      m_max_error},
//    {CC_RW_WORD, sETIME,    m_err_time},
//    {CC_RW_WORD, sDS,       m_ds},
//    {CC_RW_WORD, sMLIMIT,   m_mlimit},
//    {CC_RW_INT,  sBIAS,     m_bias},
//    {CC_RW_WORD, sASTOP,    m_astop},
//    /* Phasing Configuration Parameters */
//    {CC_RW_WORD, sPIMODE,   m_phasing_mode},
//    {CC_RW_WORD, sPITIME,   m_phasing_time},
//    {CC_RW_WORD, sPIOUT,    m_phasing_power},
//    /* Motor and Inverter Configuration Parameters */
//    {CC_RW_WORD, sPHASES,   m_motor_type},
//    {CC_RW_WORD, sPCMODE,   m_commutation_mode},
//    {CC_RW_WORD, sPPAIRS,   m_pole_pairs},
//    {CC_RW_LONG, sPCOUNTS,  m_ec_per_ecycle},
//    {CC_RW_WORD, sPMAP,     m_phase_config},
//    {CC_RW_LONG, sPORIGIN,  m_phase_origin},
//    {CC_RW_INT,  sPVECTOR,  m_phase_sector},
//    {CC_RW_INT,  sPIOFFS,   m_phase_offset},
//    {CC_RO_INT,  sPANGLE,   m_phase_angle},
//    {CC_RW_INT,  sPADV,     m_phase_adv_gain},
//    {CC_RW_INT,  sVCOMP,    m_phase_vel_comp},
//    {CC_RW_INT,  sFREQ,     m_freq},
//    {CC_RW_INT,  sABRAKE,   m_auto_brake},
//
//    {CC_RO_INT,  sIDM,      m_foc_id},
//    {CC_RO_INT,  sIQM,      m_foc_iq},
//    {CC_RO_INT,  sIQERR,    m_foc_iq_err},
//    {CC_RO_INT,  sIDERR,    m_foc_id_err},
//    {CC_RW_WORD, sQKP,      m_foc_kp},
//    {CC_RW_WORD, sQKI,      m_foc_ki},
//    {CC_RW_WORD, sQGAIN,    m_foc_qgain},
//
//    {CC_RW_WORD, sPLIMIT,   m_pc_limit},
//    {CC_RW_WORD, sPTIME,    m_pc_limit_time},
//
//    {CC_RW_WORD, sCLIMIT,   m_tc_limit},
//    {CC_RW_WORD, sCTIME,    m_tc_limit_time},
//
//    {CC_RW_LONG, sPWM,      m_outputIQ},
//    {CC_RW_LONG, sIQCMD,    m_outputIQ},
//    {CC_RW_LONG, sIDCMD,    m_outputID},
//    {CC_RW_LONG, sVU,       m_phase_voltage_U},
//    {CC_RW_LONG, sVV,       m_phase_voltage_V},
//    {CC_RW_LONG, sVW,       m_phase_voltage_W},
//    /* Encoder Parameters */
//    {CC_RW_LONG, sETYPE,    m_encoderType},
//    {CC_RW_LONG, sECPR,     m_ec_per_rev},
//    {CC_RW_HEX,  sPCT,      m_pos_capture_mode},
//    {CC_RW_LONG, sPINVERT,  m_pos_invert},
//    {CC_RW_LONG, sHPOS,     m_pos_hallchange},
//    {CC_RW_LONG, sHINVERT,  m_halls_invert},
//    {CC_RW_LONG, sHSHIFT,   m_halls_shift},
//    /* Motion Control Parameters and Commands*/
//    {CC_RW_LONG, sABS,      m_abs},
//    {CC_RW_LONG, sREL,      m_rel},
//    {CC_RW_LONG, sIND,      m_index},
//    {CC_FUNC_OA, sGO,       m_go},
//    {CC_FUNC_OA, sFWD,      m_forward},
//    {CC_FUNC_OA, sREV,      m_reverse},
//    {CC_FUNC_OA, sRESET,    m_reset},
//    {CC_FUNC,    sSERVOON,  m_servoOn},
//    {CC_FUNC,    sSERVOOFF, m_servoOff},
//    {CC_FUNC,    sENABLE,   m_powerOn},
//    {CC_FUNC,    sDISABLE,  m_powerOff},
//    {CC_FUNC,    sSTOP,     m_smoothStop},
//    {CC_FUNC,    sABORT,    m_abruptStop},
//    {CC_FUNC_OA, sALIGN,    m_alignPhase},
//    {CC_RO_STR,  sVER,      m_version},
//
//    /* Data Capture Configuration */
//    {CC_RW_WORD, sTMODE,    m_trace_mode},
//    {CC_RW_WORD, sTRATE,    m_trace_rate_mult},
//    {CC_RW_WORD, sTLEVEL,   m_trace_level},
//
//    {CC_FUNC_OA, sCH1,      m_logChannel0},
//    {CC_FUNC_OA, sCH2,      m_logChannel1},
//    {CC_FUNC_OA, sCH3,      m_logChannel2},
//    {CC_FUNC_OA, sCH4,      m_logChannel3},
//    {CC_FUNC_OA, sTRACE,    m_trace},
//    {CC_FUNC_IO, sPLAY,     m_play},
//    {CC_FUNC,    sSAVE,     m_save},
//    {CC_FUNC,    sREST,     m_restore},
//    {CC_RW_WORD, 0, 0}
//};
//
//#define TMP_BUFF_SIZE 20
//static uint8_t Buff[TMP_BUFF_SIZE];
//
//void Long2StrDec (m_console_t *pc, long value)
//{
//uint8_t * ptr = Buff + TMP_BUFF_SIZE - 1;
//bool  neg = (value < 0);
//
//    if (pc->tx_count > TX_BUF_SIZE - TMP_BUFF_SIZE - 5)
//        return;
//
//    if (neg)
//        value = -value;
//
//   *ptr-- = 0;
//   do
//   {
//      *ptr-- = (uint8_t)((value % 10) + '0');
//      value /= 10;
//   }
//   while (value != 0);
//
//   if (neg)
//      *ptr-- = '-';
//
//   while (*(++ptr))
//   {
//        *(pc->ptx++) = *ptr;
//        pc->tx_count++;
//   }
//}
//
//static uint8_t toascii (char cTemp)
//{
//   cTemp &= 15;
//   if (cTemp < 10)
//      cTemp = (char)(cTemp + '0');
//   else
//      cTemp = (char)(cTemp + 'A' - 10);
//   return (uint8_t)cTemp;
//}
//
//void Word2StrHex (m_console_t *pc, short Value)
//{
//    if (pc->tx_count > TX_BUF_SIZE - TMP_BUFF_SIZE - 5)
//        return;
//
//   *(pc->ptx++) = toascii ((char)(Value >> 12));
//   *(pc->ptx++) = toascii ((char)(Value >> 8));
//   *(pc->ptx++) = toascii ((char)(Value >> 4));
//   *(pc->ptx++) = toascii ((char)(Value));
//   pc->tx_count += 4;
//}
//
//short m_interpreter(m_console_t *pc, sf_motion_instance_t *p_motion)
//{
//    /* Parse Command and Invoke Command Handler*/
//command_t * CmdTable = Commands;
//uint8_t *pArg;
//uint8_t *pBuff = pc->rx_buf;
//bool end = false;
//int i;
//
//    while (*pBuff == ' ' || *pBuff == '\t')               // Skip white space
//        pBuff++;
//
//    if (*pBuff == '1')
//    {
//        if (*(pBuff + 1) == 0)
//            pc->channel = '1';
//        pBuff++;
//    }
//    else if (*pBuff == '2')
//    {
//        if (*(pBuff + 1) == 0)
//            pc->channel = '2';
//        pBuff++;
//    }
//
//    pc->err = 1;
//    pc->tx_count = 0;
//    pc->ptx = pc->tx_buf;
//
//    if (*pBuff == '\0')
//    {
//        pc->err = 0;
//    }
//    else
//    {
//        while (CmdTable->fptr != 0 && end == 0)
//        {
//            for (i = 0, pArg = pBuff; 1; pArg++, i++)
//            {
//                if (CmdTable->pName[i] == '\0' && (*pArg == ' ' || *pArg == '\0'))
//                {
//                    pc->err = 0;
//                    end = 1;        /* Command is found */
//                    while (*pArg == ' ' || *pArg == '=')
//                        pArg++;
//
//                    if (*pArg == '\0')                       /* Command with no argument */
//                    {
//                        int32_t v;
//                        if (CmdTable->CmdCode == CC_FUNC_IO)
//                        {
//                            pc->err = (CmdTable->fptr)(p_motion, true, (int32_t *)pc);
//                        }
//                        else
//                        {
//                            pc->err = (CmdTable->fptr)(p_motion, false, &v);
//                        }
//                        switch (CmdTable->CmdCode)
//                        {
//                        case CC_RW_WORD:
//                        case CC_RO_WORD:
//                        case CC_RW_INT:
//                        case CC_RO_INT:
//                        case CC_RW_LONG:
//                        case CC_RO_LONG:
//                            Long2StrDec (pc, v);
//                            break;
//                        case CC_RO_HEX:
//                        case CC_RW_HEX:
//                            Word2StrHex (pc, (short)v);
//                            break;
//                        case CC_RO_STR:
//                            {
//                            uint8_t *pstr = (uint8_t *)v;
//                            while (*pstr != '\0')
//                            {
//                                *(pc->ptx++) = *(pstr++);
//                                pc->tx_count++;
//                            }
//                            }
//                            break;
//                        default:
//                            break;
//                        }
//                    }
//                    else
//                    {                                   /* Command with argument */
//                        int32_t v = strtol((const char *)pArg, NULL, 0);
//                        switch (CmdTable->CmdCode)
//                        {
//                        case CC_RW_INT:
//                        case CC_RW_WORD:
//                        case CC_RW_HEX:
//                        case CC_RW_LONG:
//                        case CC_FUNC_OA:
//                            pc->err = (CmdTable->fptr)(p_motion, true, &v);
//                            break;
//                        case CC_FUNC_IO:
//                            pc->err = (CmdTable->fptr)(p_motion, true, (int32_t *)pc);
//                            break;
//                        default:
//                            break;
//                        }
//                    }
//                    break;
//                }
//                if (toupper (*pArg) != CmdTable->pName[i])
//                    break;
//            }
//            CmdTable++;
//        }
//    }
//    /* Print Prompt */
//    *(pc->ptx++) = 13;
//    *(pc->ptx++) = 10;
//    *(pc->ptx++) = pc->channel;
//    *(pc->ptx++) = pc->err == 0 ? '>' : '?';
//    pc->tx_count += 4;
//
//    return 0;
//}
