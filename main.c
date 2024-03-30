/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kameleon/board.h"

/*
 * - Can be simplified with an index if 'only' 5 modes
 *   ... but just in case we want to add more, with LEDS mix
 * - unk is unk at this time.
 */
const KAMELEON_MODE Modes[] = {
    /*[0]*/ {MODE_emulate, Modes + 1, 1 << 0},
    /*[1]*/ {MODE_rewrite, Modes + 2, 1 << 1},
    /*[2]*/ {MODE_detect, Modes + 3, 1 << 2},
    /*[3]*/ {MODE_learn, Modes + 4, 1 << 3},
    /*[4]*/ {MODE_unk, Modes + 0, 1 << 4},
};

void main(void)
{
    const KAMELEON_MODE *pMode = Modes + 0;

    BOARD_init();
    TRF7970A_init();
    LEDS_Animation();
    SLOTS_Change(Settings.CurrentSlot);

    while(true)
    {
        g_irq_SW1 = false;
        g_irq_SW2 = false;
        LEDS_MODES_Bitmask(pMode->ledsModesBitmask);
        LEDS_STATUS_Bitmask(0);
        pMode->current();
        pMode = pMode->next;
        TIMER_delay_Milliseconds(50);
    }
}
