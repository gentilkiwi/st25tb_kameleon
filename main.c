/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kameleon/board.h"

const KAMELEON_MODE Modes[] = {
    {.function = MODE_emulate, .ledsModesBitmask = 1 << 0},
    {.function = MODE_rewrite, .ledsModesBitmask = 1 << 1},
    {.function = MODE_detect,  .ledsModesBitmask = 1 << 2},
    {.function = MODE_select,  .ledsModesBitmask = 1 << 3},
    {.function = MODE_unk,     .ledsModesBitmask = 1 << 4},
};

const KAMELEON_MODE Modes_2[] = {
#if defined(ST25TB_SUPPORT_A_ST25TA512)
    {.function = MODE_emulate_14a_st25ta512_min, .ledsModesBitmask = 0b11 << 0},
#endif
#if defined(ST25TB_SUPPORT_A_NTAG210)
    {.function = MODE_emulate_14a_ntag210_min,   .ledsModesBitmask = 0b11 << 1},
#endif
    {.function = MODE_learn,   .ledsModesBitmask = 1 << 3},
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
        maxModes = count_of(Modes);
    }
    else
    {
        pMode = cMode = Modes_2;
        maxModes = count_of(Modes_2);
    }

    while(true)
    {
        g_irq_SW1 = false;
        g_irq_SW2 = false;
        LEDS_MODES_Bitmask(cMode->ledsModesBitmask);
        LEDS_STATUS_Bitmask(0);

        cMode->function();

        __no_operation();
        TRF7970A_SPI_DirectCommand(TRF79X0_STOP_DECODERS_CMD);
        __no_operation();
        __no_operation();
        TRF7970A_SPI_Write_SingleRegister(TRF79X0_CHIP_STATUS_CTRL_REG, 0x00);
        cMode++;
        if(cMode >= (pMode + maxModes))
        {
            cMode = pMode + 0;
        }
        TIMER_delay_Milliseconds(150);
    }
}
