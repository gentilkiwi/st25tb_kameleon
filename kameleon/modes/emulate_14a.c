/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "emulate_14a.h"
#include "../14a/14a_ntag210_min.h"
#include "../14a/14a_st25ta512_min.h"

/*
 * This is a Q&D code to ""emulate"" basic tag on 14A
 * - NXP NTAG210 Type 2 NDEF
 * - ST ST25TA512 Type 4 NDEF
 * Why only that? iOS compatibility
 * Why ST25TA512 after NTAG210? no emulation for 4 bits answers (?)
 *
 * More here for me to remember how to initialize some register for it
 */
void MODE_emulate_14a_ntag210_min()
{
    bool bContinueStateMachine, bExitMode = false;
    uint8_t BP_IrqSource, TRF7970A_irqStatus;
    t3RecvType type;

    LEDS_SLOTS_Bitmask(0b10101010);

    k14a_anticol_prepare_buffer(&t3a_infos_ntag210);
    do
    {
        k14a_TRF7970A_Mode();
        TRF7970A_SPI_DirectCommand(TRF79X0_RUN_DECODERS_CMD);
        do
        {
            bContinueStateMachine = false;
            BP_IrqSource = IRQ_Wait_for_SW1_or_TRF(&TRF7970A_irqStatus);
            if(BP_IrqSource & IRQ_SOURCE_TRF7970A)
            {
                type = k14a_Recv(TRF7970A_irqStatus);
                if(type == T3A_RECV_INIT)
                {
                    k14a_anticol_loop();
                    bContinueStateMachine = true;
                }
                else if(type == T3A_RECV_DATA)
                {
                    if(k14a_Dispatch_14a(&t3a_infos_ntag210, Dispatch_14a_NTAG210))
                    {
                        bContinueStateMachine = true;
                    }
                }
                else
                {
                    if((TRF7970A_irqStatus & TRF79X0_IRQ_STATUS_RF))
                    {
                        if(TRF7970A_SPI_Read_SingleRegister(TRF79X0_NFC_TARGET_PROTOCOL_REG) & TRF79X0_NFC_TARGET_PROTOCOL_RF_WAKE_UP)
                        {
                            bContinueStateMachine = true;
                        }
                    }
                }
            }
            else if(BP_IrqSource & IRQ_SOURCE_SW1)
            {
                bExitMode = true;
            }
        }
        while (bContinueStateMachine);

    }
    while (!bExitMode);

    LED_Slot(Settings.CurrentSlot);
}


void MODE_emulate_14a_st25ta512_min()
{
    bool bContinueStateMachine, bExitMode = false;
    uint8_t BP_IrqSource, TRF7970A_irqStatus;
    t3RecvType type;

    LEDS_SLOTS_Bitmask(0b01010101);

    k14a_anticol_prepare_buffer(&t3a_infos_st25ta512);
    do
    {
        k14a_TRF7970A_Mode();
        TRF7970A_SPI_DirectCommand(TRF79X0_RUN_DECODERS_CMD);
        do
        {
            bContinueStateMachine = false;
            BP_IrqSource = IRQ_Wait_for_SW1_or_TRF(&TRF7970A_irqStatus);
            if(BP_IrqSource & IRQ_SOURCE_TRF7970A)
            {
                type = k14a_Recv(TRF7970A_irqStatus);
                if(type == T3A_RECV_INIT)
                {
                    k14a_anticol_loop();
                    bContinueStateMachine = true;
                }
                else if(type == T3A_RECV_DATA)
                {
                    if(k14a_Dispatch_14a(&t3a_infos_st25ta512, Dispatch_14a_ST25TA512))
                    {
                        bContinueStateMachine = true;
                    }
                }
                else
                {
                    if((TRF7970A_irqStatus & TRF79X0_IRQ_STATUS_RF))
                    {
                        if(TRF7970A_SPI_Read_SingleRegister(TRF79X0_NFC_TARGET_PROTOCOL_REG) & TRF79X0_NFC_TARGET_PROTOCOL_RF_WAKE_UP)
                        {
                            bContinueStateMachine = true;
                        }
                    }
                }
            }
            else if(BP_IrqSource & IRQ_SOURCE_SW1)
            {
                bExitMode = true;
            }
        }
        while (bContinueStateMachine);

    }
    while (!bExitMode);

    LED_Slot(Settings.CurrentSlot);
}
