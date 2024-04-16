/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "14a.h"

uint8_t g_ui8_14a_Buffer[0x7f];
uint8_t g_ui8_cb14a_Buffer = 0;

t3RecvType k14a_Recv(uint8_t irqProvided)
{
    t3RecvType type = T3A_RECV_NONE;
    uint8_t reg;

    if (irqProvided & TRF79X0_IRQ_STATUS_SRX)
    {
        reg = TRF7970A_SPI_Read_SingleRegister(TRF79X0_NFC_TARGET_PROTOCOL_REG);
        if(((reg & (TRF79X0_NFC_TARGET_PROTOCOL_ISO14443A | TRF79X0_NFC_TARGET_PROTOCOL_ISO14443B | TRF79X0_NFC_TARGET_PROTOCOL_KBPS_MASK | TRF79X0_NFC_TARGET_PROTOCOL_FELICA)) == (TRF79X0_NFC_TARGET_PROTOCOL_ISO14443A | TRF79X0_NFC_TARGET_PROTOCOL_106KBPS)))
        {
            g_ui8_cb14a_Buffer = TRF7970A_SPI_Read_SingleRegister(TRF79X0_FIFO_STATUS_REG);
            if(g_ui8_cb14a_Buffer)
            {
                if((reg & TRF79X0_NFC_TARGET_PROTOCOL_PT_OR_CE) && (g_ui8_cb14a_Buffer == 1)) // lets assume
                {
                    type = T3A_RECV_INIT;
                }
                else if (!(g_ui8_cb14a_Buffer & TRF79X0_FIFO_OVERFLOW) && (g_ui8_cb14a_Buffer > 2))
                {
                    g_ui8_cb14a_Buffer -= 2;

                    TRF7970A_SPI_Read_ContinuousRegister(TRF79X0_FIFO_REG, g_ui8_14a_Buffer, g_ui8_cb14a_Buffer);
                    type = T3A_RECV_DATA;
                }
            }
        }
    }

    return type;
}

bool k14a_Send(const uint8_t *pcbData, const uint8_t cbData, const uint8_t bIsCRC)
{
    bool status;

    TRF7970A_SPI_Write_Packet_TYPED(pcbData, cbData, bIsCRC ? MK_DC(TRF79X0_TRANSMIT_CRC_CMD) : MK_DC(TRF79X0_TRANSMIT_NO_CRC_CMD));

    if(TRF7970A_SPI_waitIrq() & TRF79X0_IRQ_STATUS_TX)
    {
        TRF7970A_SPI_DirectCommand(TRF79X0_RESET_FIFO_CMD);
        status = true;
    }
    else
    {
        status = false;
    }

    return status;
}

const uint8_t k14a_TRF7970A_Mode_Target[] = {
    MK_WS(TRF79X0_ISO_CONTROL_REG), 0xa4,
    MK_WS(TRF79X0_RX_SPECIAL_SETTINGS_REG), 0xf0,
    MK_WS(TRF79X0_SPECIAL_FUNC_1_REG), 0x02,
    MK_WS(TRF79X0_CHIP_STATUS_CTRL_REG), TRF79X0_STATUS_CTRL_RF_ON | TRF79X0_STATUS_CTRL_RECEIVER_ON,
    MK_WS(TRF79X0_MODULATOR_CONTROL_REG), TRF79X0_MOD_CTRL_MOD_OOK_100,
    MK_WS(TRF79X0_REGULATOR_CONTROL_REG), TRF79X0_REGULATOR_CTRL_VRS_2_7V,
    MK_WS(TRF79X0_FIFO_IRQ_LEVEL), 0x0f,
    MK_WS(TRF79X0_NFC_LOW_DETECTION_LEVEL), 0x00,//0x03,
    MK_WS(TRF79X0_NFC_TARGET_LEVEL_REG), 0x07,
    MK_RS(TRF79X0_IRQ_STATUS_REG), 0x00,
};

void k14a_TRF7970A_Mode()
{
    TRF_IRQ_DISABLE();

    TRF7970A_SPI_DirectCommand(TRF79X0_SOFT_INIT_CMD);
    __delay_cycles(1);
    TRF7970A_SPI_DirectCommand(TRF79X0_IDLE_CMD);
    TIMER_delay_Milliseconds(2);

    TRF7970A_SPI_Send_raw(k14a_TRF7970A_Mode_Target, sizeof(k14a_TRF7970A_Mode_Target)); // RUN_DECODERS moved to program part

    TIMER_delay_Milliseconds(2);
    TRF_IRQ_ENABLE();
}


bool k14a_Dispatch_14a(const T3A_INFOS *pt3a_infos, PDISPATCH_14A_ROUTINE pDispatcher)
{
    bool status = true;

    uint8_t cbData = 0;
    const uint8_t *pcbData = 0;

    if((g_ui8_14a_Buffer[0] == RATS) && pt3a_infos->cbATS)
    {
        pcbData = pt3a_infos->ATS;
        cbData = pt3a_infos->cbATS;
    }
    else if((g_ui8_cb14a_Buffer == 2) &&  (g_ui8_14a_Buffer[0] == HLTA) && (g_ui8_14a_Buffer[1] == 0x00))
    {
        ;
    }
    else
    {
        if(pDispatcher)
        {
            pDispatcher(&pcbData, &cbData);
        }
    }

    if(pcbData && cbData)
    {
       if(!k14a_Send(pcbData, cbData, true))
       {
           status = false;;
       }
    }

    return status;
}
