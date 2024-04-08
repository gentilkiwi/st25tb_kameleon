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
    {MODE_emulate, 1 << 0},
    {MODE_rewrite, 1 << 1},
    {MODE_detect,  1 << 2},
    {MODE_learn,   1 << 3},
    {MODE_unk,     1 << 4},
    {MODE_emulate_14a_st25ta512_min, 0b00011},
    {MODE_emulate_14a_ntag210_min,   0b00110},
};

void main(void)
{
    uint8_t maxModes = sizeof(Modes) / sizeof(Modes[0]);
    const KAMELEON_MODE *pMode = Modes + 0;

    BOARD_init();
    TRF7970A_init();
    LEDS_Animation();
    SLOTS_Change(Settings.CurrentSlot);

    if(P1IN & BIT4) // 14A Modes only available if pushing MODE at startup
    {
        maxModes -= 2;
    }

    while(true)
    {
        g_irq_SW1 = false;
        g_irq_SW2 = false;
        LEDS_MODES_Bitmask(pMode->ledsModesBitmask);
        LEDS_STATUS_Bitmask(0);
        pMode->current();
        pMode++;
        if(pMode >= (Modes + maxModes))//(sizeof(Modes) / sizeof(Modes[0]))))
        {
            pMode = Modes + 0;
        }
        TIMER_delay_Milliseconds(50);
    }
}
