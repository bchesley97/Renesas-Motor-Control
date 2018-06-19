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
 * File Name    : hw_encoder_gpt_private.h
 * Description  : Lower level driver interface to GPT.  Register setting and reading is done here.
 **********************************************************************************************************************/

#ifndef HW_MOTOR_BLDC_PRIVATE_H
#define HW_MOTOR_BLDC_PRIVATE_H

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"

/**********************************************************************************************************************
 * Macro Definitions
 **********************************************************************************************************************/
#define     M_PI    3.14159265358979323846f
#define     M_PI_2  1.57079632679489661923f
#define     M_PI_4  0.785398163397448309616f

#define     MTR_TWOPI       (M_PI*2.0f)                 // 2*pi
#define     MTR_SQRT_2_3    (0.81649658f)               // sqrt(2/3)
#define     MTR_SQRT_3      (1.73205080f)
#define     MTR_1_SQRT_3    (1.0f/1.73205080f)
#define     MTR_1_3         (1.0f/3.0f)                 // 1/3
#define     MTR_2_3         (2.0f/3.0f)                 // 2/3

#if defined(__GNUC__) && defined(__ARM_EABI__)
#define INLINE_ATTRIBUTE __attribute__((always_inline))
#else
#define INLINE_ATTRIBUTE
#endif


/***********************************************************************************************************************
 * Private Functions
 **********************************************************************************************************************/


/*******************************************************************************************************************//**
 * Initialize channel specific registers to default values.
 * @param   channel           The channel corresponds to the hardware channel number.
 **********************************************************************************************************************/
__STATIC_INLINE void HW_MTR_RegisterInit(R_GPTA0_Type *p_gpt, uint16_t channel, uint32_t period)
{
    uint32_t half_period = period >> 1;

    p_gpt->GTWP = 0xA500;               /* Enable writing to the timer registers */
    p_gpt->GTSSR = 0x80000000;          /* Start Source Select - enable software start */
    p_gpt->GTPSR = 0;                   /* Stop Source Select */
    p_gpt->GTCSR = 0;                   /* Clear Source Select */
    p_gpt->GTUPSR = 0x00000000;         /* Up Count Source Select */
    p_gpt->GTDNSR = 0x00000000;         /* Down Count Source Select */
    p_gpt->GTICASR = 0;                 /* Input Capture Source Select Register A */
    p_gpt->GTICBSR = 0;                 /* Input Capture Source Select Register B */
    p_gpt->GTCR = 0;                    /* Control Register (start/stop) */
    p_gpt->GTUDDTYC = 1;                /* Direction and Duty setting (1 = Up) */
    p_gpt->GTINTAD = 0;                 /* Interrupt Output Setting */
    p_gpt->GTST = 0;                    /* Status */
    p_gpt->GTBER = 0;                   /* Buffer Enable Register */
    p_gpt->GTCNT = 0;                   /* Timer Counter */
    p_gpt->GTCCRA = half_period;        /* Capture/Compare Register A */
    p_gpt->GTCCRB = half_period;        /* Capture/Compare Register B */
    p_gpt->GTCCRC = half_period;        /* Capture/Compare Register C */
    p_gpt->GTCCRE = half_period;        /* Capture/Compare Register E */
    p_gpt->GTPR = period;               /* Cycle Setting Register */
    p_gpt->GTPBR = 0xFFFFFFFF;          /* Cycle Setting Buffer Register */
    p_gpt->GTDTCR = 0;                  /* Dead Time Control Register */
    p_gpt->GTDVU = 0xFFFFFFFF;          /* Dead Time Value Register */
#if BSP_MCU_GROUP_S7G2
    if (channel <= 7)
    {
        /* These registers available on GPTA only. */
        p_gpt->GTITC = 0;               /* Interrupt and ADC Start request skipping */
        p_gpt->GTPDBR = 0xFFFFFFFF;     /* Cycle Setting Double Buffer Register */
        p_gpt->GTADTRA = 0xFFFFFFFF;    /* ADC Start Request Timing A */
        p_gpt->GTADTRB = 0xFFFFFFFF;    /* ADC Start Request Timing B */
        p_gpt->GTADTBRA = 0xFFFFFFFF;   /* ADC Start Request Timing Buffer A */
        p_gpt->GTADTBRB = 0xFFFFFFFF;   /* ADC Start Request Timing Buffer B */
        p_gpt->GTADTDBRA = 0xFFFFFFFF;  /* ADC Start Request Timing Double Buffer A */
        p_gpt->GTADTDBRB = 0xFFFFFFFF;  /* ADC Start Request Timing Double Buffer B */
        p_gpt->GTDVD = 0xFFFFFFFF;
        p_gpt->GTDBU = 0xFFFFFFFF;
        p_gpt->GTDBD = 0xFFFFFFFF;
        /* GTSOS skipped - read only */
        p_gpt->GTSOTR = 0;              /* Output Protection Function Temp Release */
    }
#endif
}


/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language=extended
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning 586
#else // if defined(__CC_ARM)
#warning Not supported compiler type
#endif // if defined(__CC_ARM)

/* --------------------  End of section using anonymous unions  ------------------- */
#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
/* leave anonymous unions enabled */
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
//#pragma warning restore
#else // if defined(__CC_ARM)
#warning Not supported compiler type
#endif // if defined(__CC_ARM)

/**********************************************************************************************************************
 * Function Prototypes
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#endif /* HW_MOTOR_BLDC_PRIVATE_H */
