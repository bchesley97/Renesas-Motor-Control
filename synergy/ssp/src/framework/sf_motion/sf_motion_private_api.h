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

#ifndef SF_MOTION_PRIVATE_API_H
#define SF_MOTION_PRIVATE_API_H

/***********************************************************************************************************************
 * Private Instance API Functions. DO NOT USE! Use functions through Interface API structure instead.
 **********************************************************************************************************************/
ssp_err_t SF_MOTION_Open(sf_motion_ctrl_t * p_ctrl, sf_motion_cfg_t * p_cfg);
ssp_err_t SF_MOTION_Close(sf_motion_ctrl_t * const p_ctrl);
ssp_err_t SF_MOTION_Start(sf_motion_ctrl_t * const p_ctrl);
ssp_err_t SF_MOTION_Stop(sf_motion_ctrl_t * const p_ctrl, sf_motion_stop_mode_t stop_mode);
ssp_err_t SF_MOTION_VersionGet(ssp_version_t * const p_version);
ssp_err_t SF_MOTION_Power(sf_motion_ctrl_t * const p_ctrl, bool on);
ssp_err_t SF_MOTION_Servo(sf_motion_ctrl_t * const p_ctrl, bool on);
ssp_err_t SF_MOTION_Align(sf_motion_ctrl_t * const p_ctrl, sf_phasing_mode_t mode);
ssp_err_t SF_MOTION_Capture(sf_motion_ctrl_t * const p_ctrl, uint32_t code);
ssp_err_t SF_MOTION_Logging (sf_motion_ctrl_t *p_ctrl, int32_t code, int32_t ch);

void SF_MOTION_Control(void);

sf_motion_vpg_state_t vpg_trap_start (sf_motion_ctrl_t * const p_ctrl, bool jogging);
void vpg_trap_next (sf_motion_ctrl_t * const p_ctrl, sf_motion_profile_t *old, sf_motion_profile_t *new, bool jogging);

#endif /** SF_MOTION_PRIVATE_API_H */
