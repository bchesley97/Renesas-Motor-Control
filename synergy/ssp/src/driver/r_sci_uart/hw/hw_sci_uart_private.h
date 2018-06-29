/***********************************************************************************************************************
 * Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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
* File Name    : hw_sci_uart_private.h
* @brief    SCI HAL driver for UART
***********************************************************************************************************************/
#ifndef HW_SCI_UART_PRIVATE_H
#define HW_SCI_UART_PRIVATE_H

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
/* Includes board and MCU related header files. */
#include "bsp_api.h"
/* Public interface header file for this module. */
#include "r_sci_uart.h"

/** Common macro for SSP header files. There is also a corresponding SSP_FOOTER macro at the end of this file. */
SSP_HEADER

/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define RDR_TDAT_MASK_8BITS     (0x00FFU)
#define TDR_TDAT_MASK_9BITS     (0x01FFU)
#define RDR_TDAT_MASK_9BITS     (0x01FFU)
#define FTDR_TDAT_MASK_8BITS    (0x00FFU)
#define FTDR_TDAT_MASK_9BITS    (0x01FFU)
#define FRDR_TDAT_MASK_8BITS    (0x00FFU)
#define FRDR_TDAT_MASK_9BITS    (0x01FFU)
#define FCR_RTRG_MASK_4BITS     (0x000FU)
#define SPTR_SPB2D_BIT          (1U)
#define SPTR_OUTPUT_ENABLE_MASK (0x04U)
#define SCI_SCR_TE_MASK         (0x20U)
#define SCI_SCR_RE_MASK         (0x10U)
#define SCI_SCR_TIE_MASK        (0x80U)
#define SCI_SCR_RIE_MASK        (0x40U)
#define SCI_SCR_TEIE_MASK       (0x04U)


/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
 
/***********************************************************************************************************************
Private function prototypes
***********************************************************************************************************************/

/***********************************************************************************************************************
Private global variables
***********************************************************************************************************************/

/***********************************************************************************************************************
Private Functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @brief     Enable FIFO mode
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_FifoEnable (R_SCI0_Type * p_reg)
{
    p_reg->FCR_b.FM = 1U;    /* FCRL.FM (select FIFO mode) */
}  /* End of function HW_SCI_FifoEnable() */

/*******************************************************************************************************************//**
* @brief     Receive FIFO reset
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_ReceiveFifoReset (R_SCI0_Type * p_reg)
{
    p_reg->FCR_b.RFRST = 1U;    /* FCRL.RFRST (reset Receive FIFO Data Register) */
}  /* End of function HW_SCI_ReceiveFifoReset() */

/*******************************************************************************************************************//**
* @brief     Select RXI event which happens when detecting a reception data ready
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_RXIeventSelect (R_SCI0_Type * p_reg)
{
    p_reg->FCR_b.DRES  = 0U;    /* FCRL.DRES  (select RXI happen when detecting a reception data ready) */
}  /* End of function HW_SCI_RXIeventSelect() */

/*******************************************************************************************************************//**
* @brief     RXI happens when number of received data in FIFO becomes equal or greater than this value
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_RxTriggerNumberSet (R_SCI0_Type * p_reg, uint16_t const number)
{
    /* FCRL.RTRG  (RXI happens when number of received data in FIFO becomes equal or greater than this value) */
    p_reg->FCR_b.RTRG  = number & FCR_RTRG_MASK_4BITS;
}  /* End of function HW_SCI_RxTriggerNumberSet() */

/*******************************************************************************************************************//**
* @brief     RTS signal is asserted when number of receive data in FIFO comes equal or less than this value
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_RTSTriggerNumberSet (R_SCI0_Type * p_reg, uint16_t const number)
{
    /* FCRL.RSTRG  (RTS asserts when number of receive data comes equal or less than this value ) */
    p_reg->FCR_b.RSTRG  = (uint16_t) (number & 0xF);
}  /* End of function HW_SCI_RTSTriggerNumberSet() */

/*******************************************************************************************************************//**
* @brief     Transmit FIFO reset
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_TransmitFifoReset (R_SCI0_Type * p_reg)
{
    p_reg->FCR_b.TFRST = 1U;    /* FCRL.TFRST (reset Transmit FIFO Data Register) */
    while(p_reg->FCR_b.TFRST)
    {
        /* Wait until RFRST cleared */
    }
}  /* End of function HW_SCI_TransmitFifoReset() */

/*******************************************************************************************************************//**
* @brief     TXI happens when number of transmit data comes equal or less than this value
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_TxTriggerNumberSet (R_SCI0_Type * p_reg, uint16_t const number)
{
    /* FCRL.TTRG  (TXI happens when number of transmit data comes equal or less than this value ) */
    p_reg->FCR_b.TTRG  = number & 0xFU;
}  /* End of function HW_SCI_TxTriggerNumberSet() */

/*******************************************************************************************************************//**
* @brief     This function sets the parity based on user configuration.
* @param[in] p_reg   SCI base register
* @param[in] parity  Parity to set
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_ParitySet (R_SCI0_Type * p_reg, uart_parity_t parity)
{
    p_reg->SMR |= (uint8_t) ((uint8_t) parity << 4U);    /* Set the SMR.PE bit (Parity Enable) */
}  /* End of function HW_SCI_ParitySet() */

/*******************************************************************************************************************//**
* @brief     This function selects 7-bit data length
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_DataBits7bitsSelect (R_SCI0_Type * p_reg)
{
    /* Set the SMR.CM bit to 0 (0: Communications Mode = Asynchronous) */
    /* Set the SMR.CHR bit & SCMR.CHR1 bit as selected (Character Length)
     *  Character Length
     *  (CHR1,CHR)
     *  (1, 1) Transmit/receive in 7-bit data length*3
     */
    p_reg->SMR_b.CHR   = 1U;
    p_reg->SCMR_b.CHR1 = 1U;
}  /* End of function HW_SCI_DataBits7bitsSelect() */

/*******************************************************************************************************************//**
* @brief     This function selects 9-bit data length
* @param[in] p_reg   SCI base register
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_DataBits9bitsSelect (R_SCI0_Type * p_reg)
{
    /* Set the SMR.CM bit to 0 (0: Communications Mode = Asynchronous) */
    /* Set the SMR.CHR bit & SCMR.CHR1 bit as selected (Character Length)
     *  Character Length
     *  (CHR1,CHR)
     *  (0, 0) Transmit/receive in 9-bit data length
     */
    p_reg->SMR_b.CHR   = 0U;
    p_reg->SCMR_b.CHR1 = 0U;
}  /* End of function HW_SCI_DataBits9bitsSelect() */

/*******************************************************************************************************************//**
* @brief     This function selects stop length per user configuration
* @param[in] p_reg      SCI base register
* @param[in] stop_bits  Stop bits setting
* @retval    void
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_StopBitsSelect (R_SCI0_Type * p_reg, uart_stop_bits_t stop_bits)
{
    p_reg->SMR_b.STOP = stop_bits;
}  /* End of function HW_SCI_StopBitsSelect() */

/*******************************************************************************************************************//**
* @brief     This function writes data to transmit data register.
* @param[in] p_reg   SCI base register
* @param[in] data     Data to be sent
* @retval    void
* @note      All the parameter check must be held by HLD
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_Write9bits (R_SCI0_Type * p_reg, uint16_t const data)
{
    while (0U == p_reg->SSR_b.TDRE)
    {
        /* Wait until TDRE is cleared */
    }

    /* Write 9-bit data. */
    p_reg->TDRHL = data;

    return;
}  /* End of function HW_SCI_Write9bits() */

/*******************************************************************************************************************//**
* @brief     This function reads data from receive data register
* @param[in] channel    SCI Channel number
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE uint16_t HW_SCI_Read9bits (R_SCI0_Type * p_reg)
{
    return p_reg->RDRHL;
}  /* End of function HW_SCI_Read9bits() */

/*******************************************************************************************************************//**
* @brief     This function returns the address of the write FIFO
* @param[in] channel     SCI channel
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void * HW_SCI_WriteAddrGet (R_SCI0_Type * p_reg, uint32_t data_bytes)
{
    if (2U == data_bytes)
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void *) (&(p_reg->FTDR));
    }
    else
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void *) ((uint32_t) (&p_reg->FTDR) + 1);
    }
}  /* End of function HW_SCI_WriteAddrGet() */

/*******************************************************************************************************************//**
* @brief     This function returns the address of the write non FIFO
* @param[in] channel     SCI channel
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void * HW_SCI_Non_FIFO_WriteAddrGet (R_SCI0_Type * p_reg, uint32_t data_bytes)
{
    if (1U == data_bytes)
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void *) (&(p_reg->TDR));
    }
    else
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void *) ((uint32_t) &(p_reg->TDRHL));
    }
}  /* End of function HW_SCI_Non_FIFO_WriteAddrGet() */

/*******************************************************************************************************************//**
* @brief     This function writes data to transmit FIFO register
* @param[in] channel     SCI channel
* @param[in] data     Data to be sent
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_WriteFIFO(R_SCI0_Type * p_reg, uint16_t const data)
{
    while (0U == p_reg->SSR_b.TDRE)
    {
        /* Wait until TDRE is cleared */
    }

    /* Write data to FTDRx register */
    p_reg->FTDR = data;

    return;
}  /* End of function HW_SCI_WriteFIFO() */

/*******************************************************************************************************************//**
* @brief     This function returns the address of the read FIFO.
* @param[in] channel    SCI Channel number
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void const * HW_SCI_ReadAddrGet (R_SCI0_Type * p_reg, uint32_t data_bytes)
{
    if (2U == data_bytes)
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void const *) (&(p_reg->FRDR));
    }
    else
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void const *) ((uint32_t) &(p_reg->FRDR) + 1);
    }
}  /* End of function HW_SCI_ReadFIFO() */

/*******************************************************************************************************************//**
* @brief     This function returns the address of the read non FIFO.
* @param[in] channel    SCI Channel number
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void const * HW_SCI_Non_FIFO_ReadAddrGet (R_SCI0_Type * p_reg, uint32_t data_bytes)
{
    if (1U == data_bytes)
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void const *) (&(p_reg->RDR));
    }
    else
    {
        /* Casting for compatibility with 7 or 8 bit mode. */
        return (void const *) ((uint32_t) &(p_reg->RDRHL));
    }
}  /* End of function HW_SCI_Non_FIFO_ReadAddrGet() */

/*******************************************************************************************************************//**
* @brief     This function reads data from receive FIFO register
* @param[in] channel    SCI Channel number
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE uint16_t HW_SCI_ReadFIFO (R_SCI0_Type * p_reg)
{
    return (p_reg->FRDR) & FRDR_TDAT_MASK_9BITS;
}  /* End of function HW_SCI_ReadFIFO() */

/*******************************************************************************************************************//**
* @brief     This function clears TDFE flag
* @param[in] channel    SCI Channel number
* @retval    none
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_TDFEClear (R_SCI0_Type * p_reg)
{
    /* Clear TDFE flag */
    p_reg->SSR_FIFO_b.TDFE = 0U;
}  /* End of function HW_SCI_TDFEClear() */

/*******************************************************************************************************************//**
* @brief     This function clears RDF flag (also clears DR flag)
* @param[in] channel    SCI Channel number
* @retval    none
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_RDFClear (R_SCI0_Type * p_reg)
{
    /* Clear RDF flag */
    p_reg->SSR_FIFO_b.RDF = 0U;

    /* Clear DR flag */
    p_reg->SSR_FIFO_b.DR = 0U;

}  /* End of function HW_SCI_RDFClear() */

/*******************************************************************************************************************//**
* @brief     This function reads data number in transmit FIFO
* @param[in] channel    SCI Channel number
* @retval    Data number in transmit FIFO
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE uint32_t HW_SCI_FIFO_WriteCount (R_SCI0_Type * p_reg)
{
    return (uint32_t)(p_reg->FDR_b.T);
}  /* End of function HW_SCI_FIFO_WriteCount() */

/*******************************************************************************************************************//**
* @brief     This function reads data number in receive FIFO
* @param[in] channel    SCI Channel number
* @retval    Data number in receive FIFO
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE uint32_t HW_SCI_FIFO_ReadCount (R_SCI0_Type * p_reg)
{
    return (uint32_t)(p_reg->FDR_b.R);
}  /* End of function HW_SCI_FIFO_ReadCount() */

/*******************************************************************************************************************//**
* @brief     Enables CTS
* @param[in] p_reg   SCI base register
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_CtsInEnable (R_SCI0_Type * p_reg)
{
    p_reg->SPMR_b.CTSE = 1U;      /* enables CTS (disables RTS) */
}  /* End of function HW_SCI_CtsInEnable() */

/*******************************************************************************************************************//**
* @brief     Sets start bit detection when RXDn has falling edge if enabled is true.
*            This mode stops data reception during BREAK condition.
* @param[in] p_reg   SCI base register
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_StartBitSet (R_SCI0_Type * p_reg, bool enabled)
{
    p_reg->SEMR_b.RXDESEL = (uint8_t) (enabled & 1U);   /* detect start bit if RXDn has falling edge */
}  /* End of function HW_SCI_StartBitSet() */

/*******************************************************************************************************************//**
* Sets transmitter level.
* @param[in] p_reg   SCI base register
* @param[in] level    Default level of transmit pin, 0 is low, 1 is high
* @retval    void
* @note      Channel number is not checked in this function, caller function must check it.
***********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_TransmitterLevelSet (R_SCI0_Type * p_reg, uint8_t level)
{
    p_reg->SPTR = (uint8_t) (level << SPTR_SPB2D_BIT) | SPTR_OUTPUT_ENABLE_MASK;
}  /* End of function HW_SCI_TransmitterLevelSet() */

/*******************************************************************************************************************//**
 * Enables receive interrupt.
 * @param[in] p_reg   SCI base register
 **********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_RxIrqEnable(R_SCI0_Type * p_reg, sci_uart_instance_ctrl_t * const p_ctrl)
{
    NVIC_EnableIRQ(p_ctrl->rxi_irq);
    if (SSP_INVALID_VECTOR != p_ctrl->eri_irq)
    {
        NVIC_EnableIRQ(p_ctrl->eri_irq);
    }
    p_reg->SCR_b.RIE = 1U;
}

/*******************************************************************************************************************//**
 * Enables transmit end interrupt.
 * @param[in] p_reg   SCI base register
 **********************************************************************************************************************/
__STATIC_INLINE void HW_SCI_TeIrqEnable(R_SCI0_Type * p_reg, sci_uart_instance_ctrl_t * const p_ctrl)
{
    R_BSP_IrqStatusClear(p_ctrl->tei_irq);
    NVIC_ClearPendingIRQ(p_ctrl->tei_irq);

    uint8_t scr_temp = p_reg->SCR;
    scr_temp |= SCI_SCR_TEIE_MASK;
    scr_temp &= (uint8_t) ~SCI_SCR_TIE_MASK;
    p_reg->SCR = scr_temp;

    NVIC_DisableIRQ(p_ctrl->txi_irq);
    NVIC_EnableIRQ(p_ctrl->tei_irq);
}

__STATIC_INLINE void HW_SCI_UartTransmitterEnable (R_SCI0_Type * p_reg)
{
    p_reg->SCR |= (SCI_SCR_TE_MASK | SCI_SCR_TIE_MASK);
}  /* End of function HW_SCI_UartTransmitterEnable() */

__STATIC_INLINE void HW_SCI_UartTransmitterDisable (R_SCI0_Type * p_reg)
{
    p_reg->SCR &= (uint8_t) ~(SCI_SCR_TE_MASK | SCI_SCR_TIE_MASK | SCI_SCR_TEIE_MASK);
}

__STATIC_INLINE void HW_SCI_UartTransmitIrqDisable (R_SCI0_Type * p_reg)
{
    p_reg->SCR &= (uint8_t) ~(SCI_SCR_TIE_MASK | SCI_SCR_TEIE_MASK);
}  /* End of function HW_SCI_UartTransmitterDisable() */

__STATIC_INLINE void HW_SCI_UartReceiverEnable (R_SCI0_Type * p_reg)
{
    p_reg->SCR |= (SCI_SCR_RE_MASK | SCI_SCR_RIE_MASK);
}  /* End of function HW_SCI_UartReceiverEnable() */

__STATIC_INLINE void HW_SCI_UartTransmitterReceiverDisable (R_SCI0_Type * p_reg)
{
    p_reg->SCR &= (uint8_t) ~(SCI_SCR_TE_MASK | SCI_SCR_TIE_MASK | SCI_SCR_TEIE_MASK |
                              SCI_SCR_RE_MASK | SCI_SCR_RIE_MASK);
}  /* End of function HW_SCI_UartTransmitterReceiverDisable() */

/** Common macro for SSP header files. There is also a corresponding SSP_HEADER macro at the top of this file. */
SSP_FOOTER

#endif /* HW_SCI_UART_PRIVATE_H */
