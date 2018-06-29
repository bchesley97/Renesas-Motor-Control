#include "USB_Thread.h"
void tx_application_define_user(void * argument_name);

/* USB Thread entry function */
static char prompt[] = "Command: ";

void USB_Thread_entry(void)
{

    //tx_thread_sleep(100);

//   ssp_err_t volatile err = g_sf_comms0.p_api->open(g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg); //testing USB communication
//   while(err != TX_SUCCESS)
//   {
//       err = g_sf_comms0.p_api->open(g_sf_comms0.p_ctrl, g_sf_comms0.p_cfg); //testing USB communication
//   }

    while (1)
    {
       // g_sf_comms0.p_api->write(g_sf_comms0.p_ctrl, (uint8_t *)prompt, strlen(prompt), TX_WAIT_FOREVER); //testing USB communication
        tx_thread_sleep (1);
    }
}

//void tx_application_define_user(void * argument_name)
//{
//    if (NULL != g_sf_comms0.p_ctrl)
//    {
//    sf_el_ux_comms_instance_ctrl_t * p_ux_comms_ctrl =
//    (sf_el_ux_comms_instance_ctrl_t *) g_sf_comms0.p_ctrl;
//    /* Check if the semaphore for g_sf_comms0 is ready. */
//    if (0x53454D41 != p_ux_comms_ctrl->semaphore.tx_semaphore_id)
//    {
//    tx_semaphore_create (&p_ux_comms_ctrl->semaphore, (CHAR *)
//    "UX_COMMS_SEMAPHORE", 0);
//    }
//    }
//}
