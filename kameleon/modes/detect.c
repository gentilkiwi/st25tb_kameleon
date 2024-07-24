/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "detect.h"

void MODE_detect()
{
    uint8_t BP_IrqSource, UID[8], index;

    ST25TB_TRF7970A_Mode(true);
    do
    {
        index = SLOTS_FIND_INVALID_INDEX;
        LED_OFF(LED_INDEX_STATUS_GREEN);
        LEDS_SLOTS_Bitmask(0b00000000);

        BP_IrqSource = IRQ_Wait_for_SW1_or_SW2_or_Timeout(ST25TB_INITIATOR_DELAY_BEFORE_RETRY);
        if(BP_IrqSource & IRQ_SOURCE_TIMER)
        {
            LED_ON(LED_INDEX_STATUS_BLUE);
            BP_IrqSource = ST25TB_Initiator_Initiate_Select_UID_C1_C2(UID, NULL, NULL);
            LED_OFF(LED_INDEX_STATUS_BLUE);

            if(BP_IrqSource == IRQ_SOURCE_NONE)
            {
                LED_ON(LED_INDEX_STATUS_GREEN);
                index = SLOTS_FindByUID(UID);
                if(index != SLOTS_FIND_INVALID_INDEX)
                {
                    LED_ON(LED_INDEX_STATUS_BLUE);
                    SLOTS_Change(index);
                }

                BP_IrqSource = IRQ_Wait_for_SW1_or_SW2();
            }
        }
    }
    while (!(BP_IrqSource & IRQ_SOURCE_SW1));

    if(index == SLOTS_FIND_INVALID_INDEX)
    {
        LED_Slot(FlashStoredData.CurrentSlot);
    }
}
