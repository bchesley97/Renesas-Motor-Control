#include "uart_thread.h"
//#include "sf_uart_comms.h"
#include "sf_motion.h"
#include "motion_thread.h"
#include "mc/m_common.h"

m_console_t mc_uart;

//sf_comms_instance_t const * p_comms;
volatile ssp_err_t err;

static char input_menu[] = "Speed(RPMS): \nTorque: ";


void uart_thread_entry(void);

/* UART Thread entry function */
void uart_thread_entry(void)
{
    //g_sf_comms.p_api->open(g_sf_comms.p_ctrl, g_sf_comms.p_cfg); //open USB device framework

    while(1)
    {
       // g_sf_comms.p_api->write(g_sf_comms.p_ctrl, input_menu, strlen(input_menu), TX_NO_WAIT); //testing USB communication
        tx_thread_sleep (10);
    }



//
//    /* Set transceiver mode  to RS232 */
//    g_ioport.p_api->pinWrite(TX2EN, IOPORT_LEVEL_HIGH);
//    g_ioport.p_api->pinWrite(RS232SEL, IOPORT_LEVEL_LOW);
//    g_ioport.p_api->pinWrite(RS232_LED, 1);
//
//    p_comms = &g_sf_comms0;
//
//    err = p_comms->p_api->write(p_comms->p_ctrl, (uint8_t  const *)"Synergy Servo v.1.0\r\n1>", 23, TX_NO_WAIT);
//
//    mc_uart.channel = '1';
//    mc_uart.prx = mc_uart.rx_buf;
//    mc_uart.rx_count = 0;
//    while (1)
//    {
//        err = p_comms->p_api->read(p_comms->p_ctrl, mc_uart.prx, 1, 100);
//        if (err != SSP_SUCCESS)
//            continue;
//
//        if (*mc_uart.prx == '\r')
//        {
//            *mc_uart.prx = '\0';
//
//            /* Flush output buffer */
//            mc_uart.ptx = mc_uart.tx_buf;
//            mc_uart.tx_count = 0;
//
//            /* Invoke interpreter */
//            m_interpreter(&mc_uart, mc_uart.channel == '2' ? &g_sf_motion2 :  &g_sf_motion1);
//
//            /* Output response */
//            err = p_comms->p_api->write(p_comms->p_ctrl, mc_uart.tx_buf, mc_uart.tx_count, TX_NO_WAIT);
//
//            /* Setup receive buffer for a new transaction */
//            mc_uart.prx = mc_uart.rx_buf;
//            mc_uart.rx_count = 0;
//        }
//        else
//        {
//            mc_uart.prx++;
//            mc_uart.rx_count++;
//        }
//     }
}
