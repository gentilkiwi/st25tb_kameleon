/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "select.h"

void MODE_select()
{
    uint8_t BP_IrqSource, index = Settings.CurrentSlot;

    do
    {
        BP_IrqSource = IRQ_Wait_for_SW1_or_SW2();
        if(BP_IrqSource & IRQ_SOURCE_SW2)
        {
            index++;
            if(index >= SLOTS_COUNT)
            {
                index = 0;
            }
            SLOTS_Change(index);
        }
    }
    while (!(BP_IrqSource & IRQ_SOURCE_SW1));
}
