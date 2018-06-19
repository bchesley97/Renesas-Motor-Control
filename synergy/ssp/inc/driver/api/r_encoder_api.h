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
 * File Name    : r_encoder_api.h
 * Description  : General Encoder API.
 **********************************************************************************************************************/

#ifndef DRV_ENCODER_API_H
#define DRV_ENCODER_API_H

/*******************************************************************************************************************//**
 * @ingroup Interface_Library
 * @defgroup ENCODER_API Interface
 * @brief Interface for Encoder functions.
 *
 * @section ENCODER_API_SUMMARY Summary
 * The Encoder interface provides standard access to different position feedback mechanisms intended for motion
 * control applications. They include quardature incremental encoders supported by GPT timer, absolute encoders
 * supported by SCI or SPI interfaces, analog (SIN/COS) encoders and resolvers supported by ADCs and GPT.
 * In addition, the interface supports position capture functions which configure a trigger and
 * return the captured position value.
 *
 * Implemented by:
 * - @ref GPT
 * - @ref SCI
 * - @ref SPI
 * - @ref ADC
 *
 * See also:
 * @ref MOTOR_API
 *
 * Related SSP architecture topics:
 *  - @ref ssp-interfaces
 *  - @ref ssp-predefined-layers
 *  - @ref using-ssp-modules
 *
 * Encoder Interface description: @ref HALEncoderInterface
 * @{
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "bsp_api.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Leading zeroes removed to avoid coding standard violation. */
#define ENCODER_API_VERSION_MAJOR ((uint8_t)1)
#define ENCODER_API_VERSION_MINOR ((uint8_t)0)

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/

/** Possible status values returned by encoder_api_t::infoGet. */
typedef enum e_encoder_status
{
    ENCODER_STATUS_OK,   	 	///< Encoder interface is operational
	ENCODER_PHASE_ERROR,		///< Encoder input phases A and B have the same level
    ENCODER_COMM_ERROR,         ///< Encoder communication failure
    ENCODER_BATT_ERROR          ///< Encoder backup battery is depleted
} encoder_status_t;

/** Encoder information structure to store current status information. Refreshed every call to infoGet */
typedef struct st_encoder_info
{
    encoder_status_t    status;         ///< Current encoder status
    bool                armed;          ///< Flag indicating a position capture event is armed
    int32_t        	    counter;   	    ///< Timer counter register - current position count
    int32_t             position;       ///< Current position calculated after adding an offset to the timer counter
    int32_t             pos_last;       ///< Last position copied from pos_current before each new update cycle
    int32_t             pos_capture;    ///< Last position captured from counter register upon registration of an event
    int32_t             offset;         ///< Position offset - intended to adjust the current position on the fly

    /* Data members to reflect the status of the communication over SCI or SPI */
    uint16_t            retry_count;
    uint16_t            checksum_err_count;
    uint16_t            timeout_err_count;
} encoder_info_t;

/** User configuration structure, used in open function */
typedef struct st_encoder_cfg
{
    uint8_t             channel;        ///< Primary Channel number - Main Encoder or ADC SIN phase
    uint8_t             channel_aux;    ///< Secondary Channel number - Auxiliary Encoder (optional) or ADC COS Phase

    int32_t             ec_per_rev;     ///< Number of encoder counts per revolution
    bool                reversed;       ///< Reverse counting direction
    uint32_t            trigger;        ///< Position capture trigger

    void const          *p_extend;		///< Extension parameter for hardware specific settings.
} encoder_cfg_t;

/** Channel control block. DO NOT INITIALIZE.  Initialization occurs when encoder_api_t::open is called. */
typedef struct st_encoder_ctrl
{
    uint8_t             channel;        ///< Primary Channel number - Main Encoder or ADC SIN phase
    uint8_t             channel_aux;    ///< Secondary Channel number - Auxiliary Encoder (optional) or ADC COS Phase
    uint32_t            trigger;        ///< Position capture trigger

    encoder_info_t      crnt;           ///< Current encoder state information
    encoder_cfg_t const * p_cfg;        ///< Pointer to the configuration structure for this instance

    R_GPTA0_Type *p_gpt;
    uint32_t    open;                   ///< Whether or not channel is open
} encoder_ctrl_t;

/** Encoder API structure. General encoder functions implemented at the HAL layer follow this API. */
typedef struct st_encoder_api
{
    /** Initial configuration.
     * @par Implemented as
     * - R_ENCODER_Open()
     *
     * @pre Peripheral clocks and any required output pins should be configured prior to calling this function.
     * @note To reconfigure after calling this function, call encoder_api_t::close first.
     * @param[in]   p_ctrl     Pointer to control block. Must be declared by user. Elements set here.
     * @param[in]   p_cfg      Pointer to configuration structure. All elements of this structure must be set by user.
     */
    ssp_err_t (* open)(encoder_ctrl_t      * const p_ctrl,
                       encoder_cfg_t const * const p_cfg);

    /** Get the current counter value and translates it to encoder position. Updates internal status variables.
     * @par Implemented as
     * - R_ENCODER_PosGet()
     *
     * @param[in]   p_ctrl     Control block set in encoder_api_t::open call for this encoder.
     */
    ssp_err_t (* read)(encoder_ctrl_t * const p_ctrl);

    /** Set the current counter value.
     * @par Implemented as
     * - R_ENCODER_PosSet()
     *
     * @param[in]   p_ctrl     Control block set in encoder_api_t::open call for this encoder.
     */
    ssp_err_t (* write)(encoder_ctrl_t * const p_ctrl, int32_t pos);

    /** Set the position capture trigger code.
     * @par Implemented as
     * - R_ENCODER_PosCapSet()
     *
     * @param[in]   p_ctrl     Control block set in encoder_api_t::open call for this encoder.
     */
    ssp_err_t (* trigger)(encoder_ctrl_t * const p_ctrl, uint32_t code);

    /** Get the encoder status in the p_info structure. No hardware interactions are initiated.
     * @par Implemented as
     * - R_ENCODER_InfoGet()
     *
     * @param[in]   p_ctrl     Control block set in encoder_api_t::open call for this encoder.
     * @param[out]  p_info     Collection of information for this encoder.
     */
    ssp_err_t (* infoGet)(encoder_ctrl_t    * const p_ctrl,
                          encoder_info_t    * const p_info);

    /** Allows driver to be reconfigured and may reduce power consumption.
     * @par Implemented as
     * - R_ENCODER_Close()
     *
     * @param[in]   p_ctrl     Control block set in encoder_api_t::open call for this encoder.
     */
    ssp_err_t (* close)(encoder_ctrl_t      * const p_ctrl);

    /** Get version and store it in provided pointer p_version.
     * @par Implemented as
     * - R_ENCODER_VersionGet()
     *
     * @param[out]  p_version  Code and API version used.
     */
    ssp_err_t (* versionGet)(ssp_version_t  * const p_version);
} encoder_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_encoder_instance
{
    encoder_ctrl_t       * p_ctrl;    ///< Pointer to the control structure for this instance
    encoder_cfg_t        * p_cfg;     ///< Pointer to the configuration structure for this instance
    const encoder_api_t  * p_api;     ///< Pointer to the API structure for this instance
} encoder_instance_t;


/*******************************************************************************************************************//**
 * @} (end defgroup ENCODER_API)
 **********************************************************************************************************************/
#endif /* DRV_ENCODER_API_H */
