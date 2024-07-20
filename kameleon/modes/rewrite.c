/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "rewrite.h"

void MODE_rewrite()
{
    uint8_t BP_IrqSource;

    ST25TB_TRF7970A_Mode(true);
    do
    {
        BP_IrqSource = IRQ_Wait_for_SW1_or_SW2_or_Timeout(ST25TB_INITIATOR_DELAY_BEFORE_RETRY);
        if(BP_IrqSource & IRQ_SOURCE_TIMER)
        {
            LED_ON(LED_INDEX_STATUS_BLUE);
            BP_IrqSource = ST25TB_Initiator_Write_Current_Card(); // avoid dangerous area with ST25TB_DO_NOT_WRITE_DANGEROUS_SECTOR
            LED_OFF(LED_INDEX_STATUS_BLUE);

            if(BP_IrqSource == IRQ_SOURCE_NONE)
            {
                LED_OFF(LED_INDEX_STATUS_RED);
                LED_ON(LED_INDEX_STATUS_GREEN);

                BP_IrqSource = IRQ_Wait_for_SW1_or_SW2();

                LED_OFF(LED_INDEX_STATUS_GREEN);
            }
            else if(BP_IrqSource & (IRQ_SOURCE_TRF7970A | IRQ_SOURCE_ST25TB_PROTOCOL_ERR))
            {
                LED_ON(LED_INDEX_STATUS_RED);
            }
        }
    }
    while (!(BP_IrqSource & IRQ_SOURCE_SW1));
}
