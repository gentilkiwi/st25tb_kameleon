/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kameleon/board.h"

const KAMELEON_MODE Modes[] = {
    {MODE_emulate, 1 << 0},
    {MODE_rewrite, 1 << 1},
    {MODE_detect,  1 << 2},
    {MODE_select,  1 << 3},
    {MODE_unk,     1 << 4},
};

const KAMELEON_MODE Modes_2[] = {
    {MODE_emulate_14a_st25ta512_min, 0b11 << 0},
    {MODE_emulate_14a_ntag210_min,   0b11 << 1},
    {MODE_learn,   1 << 3},
};

void main(void)
{
    uint8_t maxModes;
    const KAMELEON_MODE *pMode, *cMode;

    BOARD_init();
    TRF7970A_init();
    LEDS_Animation();
    SLOTS_Change(Settings.CurrentSlot);

    if(P1IN & BIT4) // 14A Modes and LEARN only available if pushing MODE at startup
    {
        pMode = cMode = Modes;
        maxModes = sizeof(Modes) / sizeof(Modes[0]);
    }
    else
    {
        pMode = cMode = Modes_2;
        maxModes = sizeof(Modes_2) / sizeof(Modes_2[0]);;
    }

    while(true)
    {
        g_irq_SW1 = false;
        g_irq_SW2 = false;
        LEDS_MODES_Bitmask(cMode->ledsModesBitmask);
        LEDS_STATUS_Bitmask(0);
        cMode->current();
        cMode++;
        if(cMode >= (pMode + maxModes))
        {
            cMode = pMode + 0;
        }
        TIMER_delay_Milliseconds(150);
    }
}
