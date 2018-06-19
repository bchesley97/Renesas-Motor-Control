/***********************************************************************************************************************
 * Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
 * File Name    : sf_motion_api.h
 * Description  : Example Interface.
 **********************************************************************************************************************/

#ifndef SF_MOTION_API_H
#define SF_MOTION_API_H

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "r_motor_api.h"
#include "r_encoder_api.h"

/**********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define MAX_SF_MOTION   14               // Maximum number of sf_motion instances

/** Version of the API defined in this file */
#define SF_MOTION_API_VERSION_MAJOR (1)
#define SF_MOTION_API_VERSION_MINOR (0)

#define MOTION_VPG_FIFO_SIZE    8
#define TRACE_BUFFER_SIZE   512

/* Activity Status Flags */
#define ACT_MotionComplete  0x0001      // 1 = Trajectory generator has executed a profile
#define ACT_ServoOn         0x0002      // 1 = MotorMode On, 0 = MotorMode Off
#define ACT_PowerOn         0x0004      // 1 = Amplifier enabled, 0 = Amplifier Disabled
#define ACT_PosCaptured     0x0008      // Position captured

#define ACT_Reserved        0x0010
#define ACT_Homed           0x0020
#define ACT_Aligning        0x0040      // Phasing is ongoing
#define ACT_Aligned         0x0080      // Phasing completed

#define ACT_Busy            0x0100
#define ACT_OverCurrent     0x0200
#define ACT_Inhibit         0x0400
#define ACT_PVTEmpty        0x0800

#define ACT_OverTemp        0x1000
#define ACT_AmpFault        0x2000
#define ACT_PosError        0x4000
#define ACT_WrapAround      0x8000

/**********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/** Events that can trigger a callback function */
typedef enum e_motion_event
{
    MOTION_COMPLETED_OK,          ///< Motion completed
    MOTION_STOP_POS_ERR,
    MOTION_STOP_CURRENT_ERR,
    MOTION_STOP_POWER_ERR,
    MOTION_STOP_POS_LIMIT,
    MOTION_STOP_HW_LIMIT
} motion_event_t;

typedef enum e_motion_stop
{
    MOTION_STOP_NONE = 0,
    MOTION_STOP_ABRUPT,
    MOTION_STOP_SMOOTH,
    MOTION_STOP_SERVO_OFF,
} sf_motion_stop_mode_t;

/* Velocity Profile Generator States */
typedef enum e_motion_vpg_state {
    VPG_MotionCompleted = 0,
    VPG_TrapAcceleration,
    VPG_TrapDeceleration,
    VPG_TrapPlatoe,
    VPG_S_Profile,
    VPG_Streaming
} sf_motion_vpg_state_t;

/* Velocity Profile Generator Mode */
typedef enum e_motion_vpg_mode {
    VPGM_Trapezoidal = 0,
    VPGM_Spline,
    VPGM_Bezier,
    VPGM_PosVelTime,
    VPGM_PathPoints,
    VPGM_VelContouring,
} sf_motion_vpg_mode_t;

/* Phase Initialization Mode */
typedef enum e_phase_init_mode {
    PIM_FORCED = 0,
    PIM_HALLS_BASED,
    PIM_DITHERING,
    PIM_INDEXING,
    PIM_INDUCTIVE,
    PIM_APE_BASED,
} sf_phasing_mode_t;

typedef struct st_phasing_pars {
    sf_phasing_mode_t   mode;
    int16_t             power;
    int32_t             time;
} sf_phasing_pars_t;

typedef struct st_motion_pars {
    int32_t     position;
    int32_t     velocity;
    int32_t     acceleration;
    int32_t     deceleration;
    int32_t     accel_jerk;
    int32_t     decel_jerk;
    int16_t     dir_sign;
    sf_motion_vpg_mode_t profile_mode;
} sf_motion_pars_t;

typedef struct st_motion_profile {
    int32_t     position;
    int32_t     velocity;
    int32_t     acceleration;
    int32_t     stop_dist;
    sf_motion_vpg_state_t    vpg_state;
} sf_motion_profile_t;

typedef struct st_pid_loop {
    int16_t     kp;
    int16_t     ki;
    int16_t     kd;
    int32_t     il;
    int16_t     kvff;
    int16_t     kaff;
    int16_t     bias;
    int32_t     pos_err_limit;
    int16_t     pos_err_time_us;
    int16_t     ds_time_us;
    int16_t     ds_time_ts;
    int16_t     out_limit;
} sf_pid_loop_t;

typedef struct
{
    int16_t     mode;
    int16_t     trigger;
    int16_t     rateMult;
    uint16_t    count;
    int16_t     level;

    int16_t     code[4];
    uint16_t    *p_data[4];
    uint16_t    data[TRACE_BUFFER_SIZE * 4];

    uint16_t    index;
    int32_t     skip;
    uint16_t    cntrBegin, cntrEnd;

} sf_trace;

/** Callback function parameter data */
typedef struct st_motion_callback_args
{
    /** Placeholder for user data.  Set in motion_api_t::open function in ::motion_cfg_t. */
    void const     *p_context;
    motion_event_t  event;                      ///< The event can be used to identify what caused the callback
} sf_motion_callback_args_t;

typedef struct st_motion_cfg
{
    /* Requested Modes of operation */
    sf_motion_vpg_mode_t    profile_mode;
    sf_motion_stop_mode_t   auto_stop_mode;     ///< Auto stop mode

    sf_motion_pars_t        motion_dflt;        ///< Default Motion profile parameters - buffered
    sf_pid_loop_t           pos_loop_dflt;      ///< Position loop control PID parameters - buffered

    /* Phasing control parameters */
    sf_phasing_pars_t       phasing_pars;

    /* Amplifier interface configuration */
    uint16_t                pin_ae;             ///< Amplifier enable output
    uint16_t                pin_fault;          ///< Amplifier fault input

    /* Hall sensor feedback configuration */
    uint16_t                pin_hall_u;         ///< Phase U hall sensor input.
    uint16_t                pin_hall_v;         ///< Phase V hall sensor input.
    uint16_t                pin_hall_w;         ///< Phase W hall sensor input.

    int16_t                 halls_invert;
    int16_t                 halls_shift;

    /* Brake output */
    uint16_t                pin_brake;          ///< Brake control output.

    /* Fault control parameters */
    uint16_t                pos_err_limit_ec;   ///< Position error limit [encoder counts]
    uint16_t                pos_err_time_ms;    ///< Position error time [milliseconds]

    motor_instance_t        *p_motor;           ///< Reference to the motor control driver
    encoder_instance_t      *p_encoder;         ///< Reference to the encoder driver providing position feedback

    void (* p_callback)(sf_motion_callback_args_t * p_args); ///< Pointer to callback function
     /* Additional Configuration properties */
    void const              *p_extend;          ///< Extension parameter for hardware specific settings

} sf_motion_cfg_t;

typedef struct st_sf_motion_ctrl
{
    uint16_t                act_state;          ///< Current Activity State
    uint16_t                act_state_1;        ///< Last Activity State
    int32_t                 position;
    int32_t                 position_1;
    int32_t                 crnt_vel;
    int32_t                 position_err;
    int32_t                 integral_err;
    int32_t                 derivative_err;
    int16_t                 fault_timer;

    /* Callback for event notifications */
    void (* p_callback)(sf_motion_callback_args_t * p_args);

    /* Control parameters - run time parameters */
    sf_motion_vpg_state_t   vpg_state;          ///< Current Velocity Profile Generator State
    int32_t                 vpg_overrun;
    int32_t                 delta_pos;
    int16_t                 direction;          ///< Current direction of motion
    int32_t                 position_rel_req;   ///< Relative target position request

    sf_motion_pars_t        motion_req;         ///< Default Motion profile parameters - buffered
    sf_motion_pars_t        motion_trgt;        ///< Motion profile target parameters
    sf_motion_pars_t        motion_cmd;         ///< Motion profile command parameters

    sf_pid_loop_t           pos_loop_req;       ///< Position loop control PID parameters - buffered
    sf_pid_loop_t           pos_loop_cmd;       ///< Position loop command parameters

    int16_t                 pos_loop_time;
    int16_t                 pos_loop_timer;

    bool                    auto_brake;         ///< Flag to enable automatic control of motor brake
    int32_t                 phasing_timer;

    /* Hall Sensors Feedback */
    bool                    halls_present;
    uint16_t                hall_state;
    uint16_t                hall_state_1;
    int32_t                 hall_change_pos;
    uint16_t                hall_bit_u;
    uint16_t                hall_bit_v;
    uint16_t                hall_bit_w;

    sf_motion_profile_t     motion_profile_fifo[MOTION_VPG_FIFO_SIZE]; ///< Current motion profile data
    sf_motion_profile_t     *motion_profile_push;
    sf_motion_profile_t     *motion_profile_push_1;
    sf_motion_profile_t     *motion_profile_pull;
    uint16_t                motion_profile_points;

    /* References to related modules - motor and encoder */
    sf_motion_cfg_t         *p_cfg;
    motor_instance_t        *p_motor;           ///< Reference to the motor control driver
    const motor_api_t       *p_motor_api;       ///< Reference to the motor control functions
    motor_cfg_t             *p_motor_cfg;       ///< Reference to the motor configuration parameters
    motor_ctrl_t            *p_motor_ctrl;      ///< Reference to the motor control parameters
    motor_info_t            *p_motor_info;      ///< Reference to the motor state

    encoder_instance_t      *p_encoder;         ///< Reference to the encoder driver providing position feedback
    const encoder_api_t     *p_encoder_api;     ///< Reference to the encoder control functions
    encoder_cfg_t           *p_encoder_cfg;     ///< Reference to the encoder configuration parameters
    encoder_ctrl_t          *p_encoder_ctrl;    ///< Reference to the encoder control parameters
    encoder_info_t          *p_encoder_info;    ///< Reference to the encoder state

    sf_trace                trace;              ///< Trace support variables

    /* Additional Configuration properties */
    void const              *p_extend;          ///< Extension parameter for hardware specific settings
} sf_motion_ctrl_t;

/** Framework Periodic ADC API structure.  Implementations will use the following API. */
typedef struct st_sf_motion_api
{
    /** Open module
     * @param[in,out] p_ctrl   Pointer to a structure allocated by user. Elements initialized here.
     * @param[in]     p_cfg    Pointer to configuration structure. All elements of the structure must be set by user.
     */
    ssp_err_t (* open)(sf_motion_ctrl_t * const p_ctrl,
                       sf_motion_cfg_t * const p_cfg);

    /** Closes module
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Open.
     */
    ssp_err_t (* close)(sf_motion_ctrl_t * const p_ctrl);

    /** Gets version and stores it in provided pointer p_version.
     * @param[out]  p_version  Code and API version used.
     */
    ssp_err_t (* versionGet)(ssp_version_t * const p_version);

    /** Set controller power
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Power.
     * @param[in]    power_on Power on when set to true
     */
    ssp_err_t (* power)(sf_motion_ctrl_t * const p_ctrl, bool power_on);

    /** Set servo control
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Servo.
     * @param[in]    servo_on Servo on when set to true
     */
    ssp_err_t (* servo)(sf_motion_ctrl_t * const p_ctrl, bool servo_on);

    /** Starts motion
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Start.
     */
    ssp_err_t (* start)(sf_motion_ctrl_t * const p_ctrl);

    /** Stop motion
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Stop.
     * @param[in]    type     Motion stop method
     */
    ssp_err_t (* stop)(sf_motion_ctrl_t * const p_ctrl, sf_motion_stop_mode_t stop_mode);

    /** Initiate position capture
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Stop.
     * @param[in]    code     Position capture trigger code (register GTICASR)
     */
    ssp_err_t (* capture)(sf_motion_ctrl_t * const p_ctrl, uint32_t code);

    /** Starts phase alignment
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Align.
     * @param[in]    type     Phase alignment mode
     */
    ssp_err_t (* align)(sf_motion_ctrl_t * const p_ctrl, sf_phasing_mode_t mode);

    /** Set logging target variable
     * @param[in]    p_ctrl   Pointer to control block set in ::SF_MOTION_Power.
     * @param[in]    code     Target variable code
     * @param[in]    code     Logging channel (0..3)
     */
    ssp_err_t (* logging)(sf_motion_ctrl_t * const p_ctrl, int32_t code, int32_t ch);

} sf_motion_api_t;

/** This structure encompasses everything that is needed to use an instance of this interface. */
typedef struct st_sf_motion_instance
{
    sf_motion_ctrl_t      * p_ctrl;    ///< Pointer to the control structure for this instance
    sf_motion_cfg_t       * p_cfg;     ///< Pointer to the configuration structure for this instance
    sf_motion_api_t const * p_api;     ///< Pointer to the API structure for this instance
} sf_motion_instance_t;

/*******************************************************************************************************************//**
 * @} (end defgroup SF_MOTION_API)
 **********************************************************************************************************************/
 
#endif /* SF_MOTION_API_H */
