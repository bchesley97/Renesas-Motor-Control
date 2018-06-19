/***********************************************************************************************************************
 * Copyright [2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* File Name    : bsp.h
* Description  : Includes and API function available for this board.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_Boards
 * @defgroup BSP_BOARD_BP BSP for the S7G2 Biplane Board
 * @brief BSP for the S7G2 Biplane Board
 *
 * The S7G2_BP is a development kit for the Renesas SynergyTM S7G2 microcontroller in a LQFP176 package. It contains two
 * boards: the Main Board and the Inverter Board. The boards together provide easy-to-access interfaces and connectors
 * to the peripherals of the S7G2 for application development of Industrial Servo Controllers, Dual Ethernet Port 
 * connectivity applications as well exploring field bus interfaces such as CAN/RS485/RS422
 *
 * @{
***********************************************************************************************************************/

#ifndef BSP_H_
#define BSP_H_

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @ingroup BSP_BOARD_DK2M
 * @defgroup BSP_CONFIG_DK2M Build Time Configurations
 *
 * The BSP has multiple header files that contain build-time configuration options. Currently there are header files to
 * configure the following settings:
 *
 * - General BSP Options
 * - Clocks
 * - Interrupts
 * - Pin Configuration
 *
 * @{
 **********************************************************************************************************************/

/** @} (end defgroup BSP_CONFIG_BP) */
/* BSP Board Specific Includes. */
#include "bsp_init.h"
#include "bsp_leds.h"

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define BSP_BOARD_S7G2_BP

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
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

/***********************************************************************************************************************
Exported global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Exported global functions (to be accessed by other files)
***********************************************************************************************************************/

/** @} (end defgroup BSP_BOARD_BP) */

#endif /* BSP_H_ */
