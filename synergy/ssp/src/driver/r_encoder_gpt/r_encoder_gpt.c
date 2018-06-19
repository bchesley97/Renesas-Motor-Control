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
 * File Name    : r_encoder_gpt.c
 * Description  : GPT functions used to implement various Encoder interfaces.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "../r_gpt/hw/hw_gpt_private.h"
#include "r_encoder_gpt.h"
#include "r_encoder_gpt_cfg.h"
#include "r_encoder_gpt_private_api.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/** Maximum number of clock counts in 32 bit timer */
#define GPT_MAX_CLOCK_COUNTS_32 (0xFFFFFFFFULL)
#define GPT_MAX_CLOCK_COUNTS_16 (0xFFFFUL)

/** Macro for error logger. */
#ifndef GPT_ERROR_RETURN
#define GPT_ERROR_RETURN(a, err) SSP_ERROR_RETURN((a), (err), "encoder_gpt", &g_encoder_version)
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
#if defined(__GNUC__)
/* This structure is affected by warnings from the GCC compiler bug https://gcc.gnu.org/bugzilla/show_bug.cgi?id=60784
 * This pragma suppresses the warnings in this structure only, and will be removed when the SSP compiler is updated to
 * v5.3.*/
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif


/*******************************************************************************************************************//**
 * Initialize channel specific registers to default values.
 * @param   channel           The channel corresponds to the hardware channel number.
 **********************************************************************************************************************/
__STATIC_INLINE void HW_ENC_RegisterInit(encoder_ctrl_t * const p_ctrl)
{
    R_GPTA0_Type *p_gpt = p_ctrl->p_gpt;

    p_gpt->GTWP = 0xA500;               /* Enable writing to the timer registers */
    p_gpt->GTSSR = 0;                   /* Start Source Select */
    p_gpt->GTPSR = 0;                   /* Stop Source Select */
    p_gpt->GTCSR = 0;                   /* Clear Source Select */
    p_gpt->GTUPSR = 0x00006900;         /* Up Count Source Select */
    p_gpt->GTDNSR = 0x00009600;         /* Down Count Source Select */
    p_gpt->GTICASR = 0;                 /* Input Capture Source Select Register A */
    p_gpt->GTICBSR = 0;                 /* Input Capture Source Select Register B */
    p_gpt->GTCR = 1;                    /* Control Register (start/stop) */
    p_gpt->GTUDDTYC = 1;                /* Direction and Duty setting (1 = Up) */
    p_gpt->GTIOR = 0;                   /* I/O Control */
    p_gpt->GTINTAD = 0;                 /* Interrupt Output Setting */
    p_gpt->GTST = 0;                    /* Status */
    p_gpt->GTBER = 0;                   /* Buffer Enable Register */
    p_gpt->GTCNT = 0;                   /* Timer Counter */
    p_gpt->GTCCRA = 0xFFFFFFFF;         /* Capture/Compare Register A */
    p_gpt->GTCCRB = 0xFFFFFFFF;         /* Capture/Compare Register B */
    p_gpt->GTCCRC = 0xFFFFFFFF;         /* Capture/Compare Register C */
    p_gpt->GTCCRE = 0xFFFFFFFF;         /* Capture/Compare Register E */
    p_gpt->GTPR = 0xFFFFFFFF;           /* Cycle Setting Register */
    p_gpt->GTPBR = 0xFFFFFFFF;          /* Cycle Setting Buffer Register */
    p_gpt->GTDTCR = 0;                  /* Dead Time Control Register */
    p_gpt->GTDVU = 0xFFFFFFFF;          /* Dead Time Value Register */
}


/** Version data structure used by error logger macro. */
static const ssp_version_t g_encoder_gpt_version =
{
    .api_version_minor  = ENCODER_API_VERSION_MINOR,
    .api_version_major  = ENCODER_API_VERSION_MAJOR,
    .code_version_major = ENCODER_GPT_CODE_VERSION_MAJOR,
    .code_version_minor = ENCODER_GPT_CODE_VERSION_MINOR
};
#if defined(__GNUC__)
/* Restore warning settings for 'missing-field-initializers' to as specified on command line. */
/*LDRA_INSPECTED 69 S */
#pragma GCC diagnostic pop
#endif

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/
/** GPT Implementation of General Timer Driver  */
const encoder_api_t g_encoder_on_gpt =
{
    .open            = R_ENCODER_Open,
    .read            = R_ENCODER_PosGet,
    .trigger         = R_ENCODER_PosCapSet,
    .write           = R_ENCODER_PosSet,
    .infoGet         = R_ENCODER_InfoGet,
    .close           = R_ENCODER_Close,
    .versionGet      = R_ENCODER_VersionGet
};

/*******************************************************************************************************************//**
 * @addtogroup ENCODER
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

static ssp_err_t gpt_common_open (encoder_ctrl_t * const p_ctrl,
                                  encoder_cfg_t   const * const p_cfg,
                                  ssp_feature_t * p_ssp_feature)
{
    fmi_feature_info_t info = {0U};

    ssp_err_t     err          = SSP_SUCCESS;
    p_ssp_feature->channel = p_cfg->channel;
    err = g_fmi_on_fmi.productFeatureGet(p_ssp_feature, &info);
    GPT_ERROR_RETURN(SSP_SUCCESS == err, err);
    p_ctrl->p_gpt = info.ptr;

    return SSP_SUCCESS;
} /* End of function gpt_common_open */


/*******************************************************************************************************************//**
 * @brief  Powers on GPT, handles required initialization described in hardware manual. Implements timer_api_t::open.
 *
 * The Open function configures a single GPT channel, starts the channel, and provides a handle for use with the GPT API
 * Control and Close functions. This function must be called once prior to calling any other GPT API functions. After a
 * channel is opened, the Open function should not be called again for the same channel without first calling the
 * associated Close function.
 *
 * GPT hardware does not support one-shot functionality natively.  When using one-shot mode, the timer will be stopped
 * in an ISR after the requested period has elapsed.
 *
 * The GPT implementation of the general timer can accept a ::timer_on_gpt_cfg_t extension parameter.
 *
 * @retval SSP_SUCCESS           Initialization was successful and timer has started.
 * @retval SSP_ERR_ASSERTION     One of the following parameters is incorrect.  Either
 *                                 - p_cfg is NULL, OR
 *                                 - p_ctrl is NULL, OR
 *                                 - The channel requested in the p_cfg parameter is not available on the device
 *                                   selected in r_bsp_cfg.h.
 * @retval SSP_ERR_INVALID_ARGUMENT  One of the following parameters is invalid:
 *                                 - p_cfg->period: must be in the following range:
 *                                     - Lower bound: (1 / (PCLK frequency)
 *                                     - Upper bound: (0xFFFFFFFF * 1024 / (PCLK frequency))
 *                                 - p_cfg->p_callback not NULL, but ISR is not enabled. ISR must be enabled to
 *                                   use callback function.  Enable channel's overflow ISR in bsp_irq_cfg.h.
 *                                 - p_cfg->mode is ::TIMER_MODE_ONE_SHOT, but ISR is not enabled.  ISR must be
 *                                   enabled to use one-shot mode.  Enable channel's overflow ISR in bsp_irq_cfg.h.
 * @retval SSP_ERR_IN_USE        The channel specified has already been opened. No configurations were changed. Call
 *                               the associated Close function or use associated Control commands to reconfigure the
 *                               channel.
 *
 * @note This function is reentrant for different channels.  It is not reentrant for the same channel.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_Open (encoder_ctrl_t * const      p_ctrl,
                          encoder_cfg_t const * const p_cfg)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /* Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
#endif
    p_ctrl->channel = p_cfg->channel;
    p_ctrl->p_cfg = p_cfg;
    p_ctrl->crnt.offset = 0;

    p_ctrl->crnt.status = 0;
    p_ctrl->crnt.retry_count = p_ctrl->crnt.timeout_err_count = 0;
    p_ctrl->crnt.position = p_ctrl->crnt.pos_last = 0;

    ssp_feature_t ssp_feature = {{(ssp_ip_t) 0}};
    ssp_feature.channel = p_cfg->channel;
    ssp_feature.unit = 0U;
    ssp_feature.id = SSP_IP_GPT;

    ssp_err_t err = gpt_common_open(p_ctrl, p_cfg, &ssp_feature);
    GPT_ERROR_RETURN((SSP_SUCCESS == err), err);

    /** Verify channel is not already used */
    ssp_err_t     bsp_err = R_BSP_HardwareLock(&ssp_feature);
    GPT_ERROR_RETURN((SSP_SUCCESS == bsp_err), SSP_ERR_IN_USE);

    /** Power on GPT before setting any hardware registers. Make sure the counter is stopped before setting mode
       register, PCLK divisor register, and counter register. */
    GPT_BASE_PTR p_gpt_reg = (GPT_BASE_PTR) p_ctrl->p_gpt;
    R_BSP_ModuleStart(&ssp_feature);

    /* Initialize the GPT as Quadrature decoder */
    HW_ENC_RegisterInit(p_ctrl);
    /* Arm position capture */
    R_ENCODER_PosCapSet(p_ctrl, p_cfg->trigger);
    /* Enable the POE module to be able to use the external trigger */
    R_MSTP->MSTPCRD_b.MSTPD14 = 0;
    /* Start counter */
    HW_GPT_CounterStartStop(p_gpt_reg, GPT_START);

    p_ctrl->open = 1;
    return SSP_SUCCESS;
} /* End of function R_ENCODER_TimerOpen */

/*******************************************************************************************************************//**
 * @brief  Gets counter value in provided p_value pointer. Implements timer_api_t::posGet.
 *
 * @retval SSP_SUCCESS           Counter value read, p_value is valid.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_PosGet (encoder_ctrl_t * const p_ctrl)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /* Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_gpt);
#endif

    /* Read counter raw value */
    p_ctrl->crnt.counter = (int32_t)(p_ctrl->p_gpt->GTCNT);
    /* Add position offset */
    p_ctrl->crnt.position = p_ctrl->crnt.counter + p_ctrl->crnt.offset;

    /* Check if a new position is captured */
    if (p_ctrl->p_gpt->GTST_b.TCFA != 0 && p_ctrl->crnt.armed)
    {
        /* Read captured counter raw value */
        p_ctrl->crnt.pos_capture = (int32_t)(p_ctrl->p_gpt->GTCCRA) + p_ctrl->crnt.offset;
        p_ctrl->crnt.armed = false;
        /* Clear position captured flag */
        p_ctrl->p_gpt->GTST_b.TCFA = 0;
    }
    return SSP_SUCCESS;
} /* End of function R_ENCODER_PosGet */

/*******************************************************************************************************************//**
 * @brief  Gets captured counter value in provided p_value pointer. Implements timer_api_t::posCapGet.
 *
 * @retval SSP_SUCCESS           Counter value read, p_value is valid.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_PosCapSet (encoder_ctrl_t * const p_ctrl, uint32_t code)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /* Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_gpt);
#endif

    /* Arm position capture trigger with the new code if different than zero */
    if (code != 0)
    {
        p_ctrl->p_gpt->GTICASR = code;
        p_ctrl->trigger = code;
    }
    /* Clear position captured status / set armed status */
    p_ctrl->crnt.armed = true;
    /* Clear position captured flag */
    p_ctrl->p_gpt->GTST_b.TCFA = 0;
    return SSP_SUCCESS;
} /* End of function R_ENCODER_PosCapSet */

/*******************************************************************************************************************//**
 * @brief  Sets counter value in provided p_value pointer. Implements timer_api_t::counterGet.
 *
 * @retval SSP_SUCCESS           Counter value read, p_value is valid.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_value parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_PosSet (encoder_ctrl_t * const p_ctrl, int32_t pos)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /* Make sure parameters are valid */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_ctrl->p_gpt);
#endif

    /* Read counter raw value */
    //p_ctrl->counter = (int32_t)(p_ctrl->p_gpt->GTCNT);
    /* Update position offset */
    p_ctrl->crnt.offset = pos - p_ctrl->crnt.counter;
    /* TODO: Update the info structure */

    return SSP_SUCCESS;
} /* End of function R_ENCODER_PosGet */

/*******************************************************************************************************************//**
 * @brief  Get timer information and store it in provided pointer p_info.
 * Implements timer_api_t::infoGet.
 *
 * @retval SSP_SUCCESS           Period, count direction, frequency, and status value written to caller's
 *                               structure successfully.
 * @retval SSP_ERR_ASSERTION     The p_ctrl or p_info parameter was null.
 * @retval SSP_ERR_NOT_OPEN      The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_InfoGet (encoder_ctrl_t * const p_ctrl, encoder_info_t * const p_info)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
    SSP_ASSERT(NULL != p_info);
#endif
    /* The info structure is store in the control block */
    *p_info = p_ctrl->crnt;

    return SSP_SUCCESS;
} /* End of function R_ENCODER_InfoGet */

/*******************************************************************************************************************//**
 * @brief      Stops counter, disables interrupts, disables output pins, and clears internal driver data.
 *
 * @retval     SSP_SUCCESS          Successful close.
 * @retval     SSP_ERR_ASSERTION    The parameter p_ctrl is NULL.
 * @retval     SSP_ERR_NOT_OPEN     The channel is not opened.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_Close (encoder_ctrl_t * const p_ctrl)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /** Make sure handle is valid. */
    SSP_ASSERT(NULL != p_ctrl);
#endif
    return SSP_SUCCESS;
} /* End of function R_ENCODER_Close */

/*******************************************************************************************************************//**
 * @brief      Sets driver version based on compile time macros.
 *
 * @retval     SSP_SUCCESS          Successful close.
 * @retval     SSP_ERR_ASSERTION    The parameter p_version is NULL.
 **********************************************************************************************************************/
ssp_err_t R_ENCODER_VersionGet (ssp_version_t * const p_version)
{
#if ENCODER_GPT_CFG_PARAM_CHECKING_ENABLE
    /** Verify parameters are valid */
    SSP_ASSERT(NULL != p_version);
#endif

    p_version->version_id = 1;

    return SSP_SUCCESS;
} /* End of function R_ENCODER_VersionGet */

/** @} (end addtogroup GPT) */

