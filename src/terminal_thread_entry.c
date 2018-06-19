
/************************************************
  includes
 ***********************************************/

#include <terminal_thread.h>

/************************************************
 global variables
 ***********************************************/
//static const char input_menu[] = "Speed(rpms): \nTorque: ";
//char rx_chars[20];

/* Termnal Thread entry function */
void terminal_thread_entry(void)
{


    //g_sf_comms.p_api->open(g_sf_comms.p_ctrl, g_sf_comms.p_cfg); //open terminal communication framework


    while (1)
    {

       // g_sf_comms.p_api->write(g_sf_comms.p_ctrl, input_menu, strlen(input_menu), TX_WAIT_FOREVER);
        tx_thread_sleep (1);

      //  g_sf_comms.p_api->read(g_sf_comms.p_ctrl, rx_chars, sizeof(rx_chars), TX_WAIT_FOREVER);
        tx_thread_sleep (20);

    }
}
