#include "motion_thread.h"
bsp_leds_t Leds;


void motion_thread_entry(void);

static int32_t led_run_toggle = 0;
static int32_t led_run_cnt = 0;










/* Motion Thread entry function */
void motion_thread_entry(void)
{
    /* Get LED information for this board */
    R_BSP_LedsGet(&Leds);




    /* If this board has no LEDs then trap here */
    if (!Leds.led_count)
    {
        while(1);   // There are no LEDs on this board
    }

    //ioport_level_t level = IOPORT_LEVEL_HIGH;

    /* TODO: add your own code here */
    while (1)
    {
//        if (++led_run_cnt >= 3)
//        {
//            led_run_cnt = 0;
//            led_run_toggle ^= 1;
//            g_ioport.p_api->pinWrite(RUN_LED, (ioport_level_t)led_run_toggle);
//        }
//        /* Display amplifier fault status */
//        g_ioport.p_api->pinWrite(FAULT1_LED, (g_sf_motion1.p_ctrl->act_state & ACT_AmpFault) ? 1 : 0);
//        g_ioport.p_api->pinWrite(FAULT2_LED, (g_sf_motion2.p_ctrl->act_state & ACT_AmpFault) ? 1 : 0);
//
//        /* Display servo/motion status */
//        g_ioport.p_api->pinWrite(ENC1_ON_LED, (g_sf_motion1.p_ctrl->act_state & ACT_ServoOn) ?
//                                                ((g_sf_motion1.p_ctrl->act_state & ACT_MotionComplete) ? 1 : led_run_toggle) : 0);
//        g_ioport.p_api->pinWrite(ENC2_ON_LED, (g_sf_motion2.p_ctrl->act_state & ACT_ServoOn) ?
//                                                ((g_sf_motion2.p_ctrl->act_state & ACT_MotionComplete) ? 1 : led_run_toggle) : 0);
//        /* Display axis error status */
//        g_ioport.p_api->pinWrite(ENC1_ON_LED, (g_sf_motion1.p_ctrl->act_state & (ACT_PosError|ACT_OverCurrent|ACT_WrapAround|ACT_OverTemp|ACT_Inhibit)) ?
//                                                ((g_sf_motion1.p_ctrl->act_state & ACT_PosError) ? led_run_toggle : 1) : 0);
//        g_ioport.p_api->pinWrite(ENC2_ON_LED, (g_sf_motion2.p_ctrl->act_state & (ACT_PosError|ACT_OverCurrent|ACT_WrapAround|ACT_OverTemp|ACT_Inhibit)) ?
//                                                ((g_sf_motion2.p_ctrl->act_state & ACT_PosError) ? led_run_toggle : 1) : 0);
        tx_thread_sleep (10);
    }
}
