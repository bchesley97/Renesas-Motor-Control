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
 * File Name    : sf_motion_vpg.c
 * Description  : Motion Control Framework.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <framework/instances/sf_motion.h>
#include "sf_motion_private_api.h"

sf_motion_vpg_state_t vpg_trap_start (sf_motion_ctrl_t * const p_ctrl, bool jogging)
{
    p_ctrl->motion_profile_push_1 = &p_ctrl->motion_profile_fifo[0];
    p_ctrl->motion_profile_push_1->position = p_ctrl->motion_cmd.position;
    p_ctrl->motion_profile_push_1->velocity = 0;
    p_ctrl->motion_profile_push_1->acceleration = 0;
    p_ctrl->motion_profile_push_1->stop_dist = 0;
    p_ctrl->motion_profile_push_1->vpg_state = VPG_MotionCompleted;

    p_ctrl->motion_profile_push = p_ctrl->motion_profile_push_1 + 1;
    p_ctrl->motion_profile_pull = p_ctrl->motion_profile_push;
    p_ctrl->delta_pos = 0;

    if (jogging)  /* Jogging? */
    {
        p_ctrl->direction = p_ctrl->motion_trgt.dir_sign;
    }
    else
    {
        p_ctrl->direction = p_ctrl->motion_trgt.position >= p_ctrl->motion_profile_push_1->position ? 1 : -1;
    }
    vpg_trap_next(p_ctrl, p_ctrl->motion_profile_push_1, p_ctrl->motion_profile_push, jogging);
    p_ctrl->motion_profile_push_1 = p_ctrl->motion_profile_push;
    p_ctrl->motion_profile_push++;
    p_ctrl->motion_profile_points = 1;
    return p_ctrl->motion_profile_push_1->vpg_state;
}

void vpg_trap_next (sf_motion_ctrl_t * const p_ctrl, sf_motion_profile_t *crnt_cmd, sf_motion_profile_t *new_cmd, bool jogging)
{
    long dist2target;
    long temp32;
    int16_t temp16, dir_sign;

        if (jogging)   /* Jogging? */
        {
            dir_sign = p_ctrl->motion_trgt.dir_sign;
            dist2target = 0x7fffffff;
            if (p_ctrl->motion_trgt.velocity == 0 && crnt_cmd->velocity < p_ctrl->motion_trgt.deceleration)
            {
                new_cmd->velocity = 0;
                new_cmd->acceleration = 0;
                new_cmd->position = crnt_cmd->position;
                new_cmd->vpg_state = VPG_MotionCompleted;
                return;
            }
        }
        else
        {
            dist2target = p_ctrl->motion_trgt.position - crnt_cmd->position;
            if (dist2target < 0)
            {
                dir_sign = -1;
                dist2target = -dist2target;
            }
            else
            {
                dir_sign = 1;
            }
            temp16 = (int16_t)(p_ctrl->motion_trgt.deceleration >> 16);
            if (temp16 < 1)
                temp16 = 1;
            if (dist2target <= (long) temp16)
            {
                 new_cmd->velocity = 0;
                 new_cmd->acceleration = 0;
                 new_cmd->position = p_ctrl->motion_trgt.position;
                 new_cmd->vpg_state = VPG_MotionCompleted;
                 return;
            }
        }
        if (((crnt_cmd->stop_dist >= dist2target) ||
            (crnt_cmd->velocity > p_ctrl->motion_trgt.velocity) ||
            (p_ctrl->direction != dir_sign)))
        {
            /* Deceleration Phase */
            temp32 = crnt_cmd->velocity - p_ctrl->motion_trgt.deceleration;
            if (temp32 < p_ctrl->motion_trgt.deceleration) {
                p_ctrl->direction = dir_sign;
                if (p_ctrl->motion_trgt.velocity < p_ctrl->motion_trgt.deceleration)
                {
                    new_cmd->velocity = p_ctrl->motion_trgt.velocity;
                }
                else
                {
                    new_cmd->velocity = p_ctrl->motion_trgt.deceleration;
                }
            }
            else
            {
                new_cmd->velocity = temp32;
            }
            if (crnt_cmd->velocity > p_ctrl->motion_trgt.velocity)
                new_cmd->velocity = temp32 < p_ctrl->motion_trgt.velocity ? p_ctrl->motion_trgt.velocity : temp32;

            new_cmd->acceleration = p_ctrl->motion_trgt.deceleration;
            new_cmd->vpg_state = VPG_TrapDeceleration;
        }
        else if (crnt_cmd->velocity < p_ctrl->motion_trgt.velocity)
        {
            /* acceleration Phase */
            temp32 = crnt_cmd->velocity + p_ctrl->motion_trgt.acceleration;
            new_cmd->velocity = temp32 > p_ctrl->motion_trgt.velocity ? p_ctrl->motion_trgt.velocity : temp32;
            new_cmd->acceleration = p_ctrl->motion_trgt.acceleration;
            new_cmd->vpg_state = VPG_TrapAcceleration;
        }
        else
        {
            /* Plateau */
            new_cmd->velocity = crnt_cmd->velocity;
            new_cmd->acceleration = 0;
            new_cmd->vpg_state = VPG_TrapPlatoe;
        }

        /* Calculate stopping distance = (velocity * velocity) / deceleration */
        new_cmd->stop_dist = (long)(((float)new_cmd->velocity * (float)new_cmd->velocity) / (float)(p_ctrl->motion_trgt.deceleration<<1)) >> 16;

        /* Calculate commanded position via trapezoidal integration of the velocity */
        p_ctrl->delta_pos += (crnt_cmd->velocity + new_cmd->velocity) >> 1;
        temp32 = (p_ctrl->delta_pos >> 16);
        if (p_ctrl->direction < 0)
            temp32 = -temp32;
        new_cmd->position = crnt_cmd->position + temp32;
        p_ctrl->delta_pos &= 0x0000ffff;
        return;
}

