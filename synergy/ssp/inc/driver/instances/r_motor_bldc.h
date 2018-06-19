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
 * File Name    : r_motor_bldc.h
 * Description  : Prototypes of BLDC Motor functions used to implement various Motor interfaces.
 **********************************************************************************************************************/

#ifndef R_MOTOR_BLDC_H
#define R_MOTOR_BLDC_H

/*******************************************************************************************************************//**
 * @ingroup HAL_Library
 * @defgroup GPT GPT
 * @brief Driver for the BLDC Motor.
 *
 * @section GPT_SUMMARY Summary
 * Extends @ref TIMER_API.
 *
 * This module implements the @ref TIMER_API using the General PWM Timer (GPT) peripherals GPT32EH, GPT32E, GPT32.
 * It also provides an output compare
 * extension to output the timer signal to the GTIOC pin.
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_motor_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MOTOR_BLDC_CODE_VERSION_MAJOR (1)
#define MOTOR_BLDC_CODE_VERSION_MINOR (0)

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
/** @cond INC_HEADER_DEFS_SEC */
/** Filled in Interface API structure for this Instance. */
extern const motor_api_t g_motor_bldc;
/** @endcond */

/*******************************************************************************************************************//**
 * @} (end defgroup MOTOR_BLDC)
 **********************************************************************************************************************/
#endif /* R_MOTOR_BLDC_H */
