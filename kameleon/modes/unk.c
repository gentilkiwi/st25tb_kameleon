/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "unk.h"

void MODE_unk()
{
    uint8_t BP_IrqSource, Counter1[4];

    ST25TB_TRF7970A_Mode(true);
    do
    {
        LEDS_STATUS_Bitmask(0b000);
        LEDS_SLOTS_Bitmask(0b00000000);

        BP_IrqSource = IRQ_Wait_for_SW1_or_SW2_or_Timeout(ST25TB_INITIATOR_DELAY_BEFORE_RETRY);
        if(BP_IrqSource & IRQ_SOURCE_TIMER)
        {
            LED_ON(LED_INDEX_STATUS_BLUE);
            BP_IrqSource = ST25TB_Initiator_Initiate_Select_UID_C1_C2(NULL, Counter1, NULL);
            LED_OFF(LED_INDEX_STATUS_BLUE);

            if(BP_IrqSource == IRQ_SOURCE_NONE)
            {
                if(!Counter1[0]) // Empty ticket
                {
                    LED_ON(LED_INDEX_STATUS_RED);
                    LED_Slot(0);
                }
                else if(Counter1[0] < 8) // Can be displayed on single LED slot
                {
                    LED_ON(LED_INDEX_STATUS_GREEN);
                    LED_Slot(Counter1[0]);
                }
                else // >= 8, it will use binary to display ON LED slots
                {
                    LED_ON(LED_INDEX_STATUS_BLUE);
                    LEDS_SLOTS_Bitmask(Counter1[0]);
                }

                BP_IrqSource = IRQ_Wait_for_SW1_or_SW2();
            }
        }
    }
    while (!(BP_IrqSource & IRQ_SOURCE_SW1));

    LED_Slot(FlashStoredData.CurrentSlot);
}
