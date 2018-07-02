/* generated configuration header file - do not edit */
#ifndef BSP_PIN_CFG_H_
#define BSP_PIN_CFG_H_

/***********************************************************************************************************************
 Pin Configuration Table
 ***********************************************************************************************************************/

/* This conditional makes sure that this table is only allocated once. Without this check the table would be allocated
 * in every module that includes this header file. */
#if defined(BSP_PIN_ALLOCATE)

/* Used for getting IOPORT types. */
#include "r_ioport.h"

/** Array of pin configurations. */
const ioport_pin_cfg_t g_bsp_pin_cfg_data[] =
{
    {
        .pin = IOPORT_PORT_01_PIN_08,
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = IOPORT_PORT_01_PIN_09,
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = IOPORT_PORT_01_PIN_10,
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_DEBUG),
    },
    {
        .pin = IOPORT_PORT_03_PIN_00,
        .pin_cfg = (IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_DEBUG),
    },
};

/** Has pointer to pin configuration array and number of pins to be configured. */
const ioport_cfg_t g_bsp_pin_cfg =
{
    .number_of_pins = sizeof(g_bsp_pin_cfg_data)/sizeof(ioport_pin_cfg_t),
    .p_pin_cfg_data = &g_bsp_pin_cfg_data[0],
};

#endif /* BSP_PIN_ALLOCATE */

#endif /* BSP_PIN_CFG_H_ */
