/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "board.h"

volatile bool g_irq_TA0, g_irq_SW1, g_irq_SW2, g_irq_TRF;

#pragma PERSISTENT(Settings)
GLOBAL_SETTINGS Settings = {
    .CurrentSlot = 0,
};

void BOARD_init()
{
    WDTCTL = WDTPW | WDTHOLD;
    PM5CTL0 &= ~LOCKLPM5;

    /*
     * MCU @ 16 MHz
     */
    __bis_SR_register(SCG0);
    CSCTL0 = ~DCO;
    CSCTL1 = DCOFTRIMEN_0 | DCORSEL_5 | DISMOD_1; // DCORSEL_5 for 16 MHz
    CSCTL2 = FLLD__1 | (16000000 / 32768);
    __bic_SR_register(SCG0);
    __delay_cycles((16000000 / 32768) * 32 * 30);
    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1 | DCOFFG))
    {
        CSCTL7 &= ~DCOFFG;
        SFRIFG1 &= ~OFIFG;
    }

    /*
     * P1.0 LED_MOD_LEARN    output        |  P2.0 SPI_CS           output        |  P3.0 LED_SLOT3        output        |  P4.0 LED_STATUS_GREEN output        *
     * P1.1 LED_MOD_UNK      output        |  P2.1 TRF_IRQ          input/irq     |  P3.1 LED_SLOT7        output        |  P4.1 LED_STATUS_RED   output        *
     * P1.2 LED_SLOT0        output        |  P2.2 LED_SLOT2        output        |  P3.2 SPI_MOSI         output/spi    |  P4.2 SW2              input/pullup  *
     * P1.3 LED_SLOT1        output        |  P2.3 LED_SLOT5        output        |  P3.3 LED_SLOT4        output        |  P4.3 n/a              output        *
     * P1.4 SW1              input/pullup  |  P2.4 n/a              output        |  P3.4 LED_SLOT6        output        |  P4.4 n/a              output        *
     * P1.5 LED_MOD_EMULATE  output        |  P2.5 n/a              output        |  P3.5 SPI_CLK          output/spi    |  P4.5 n/a              output        *
     * P1.6 LED_MOD_REWRITE  output        |  P2.6 n/a              output        |  P3.6 SPI_MISO         input/spi     |  P4.6 n/a              output        *
     * P1.7 LED_MOD_DETECT   output        |  P2.7 TRF_EN           output        |  P3.7 LED_STATUS_BLUE  output        |  P4.7 n/a              output        *
     */
    P1DIR = BIT0 | BIT1 | BIT2 | BIT3 | /*BIT4 | */BIT5 | BIT6 | BIT7;
    P2DIR = BIT0 | /*BIT1 | */BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7;
    P3DIR = BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | /*BIT6 | */BIT7;
    P4DIR = BIT0 | BIT1 | /*BIT2 | */BIT3 | /*BIT4 | */BIT5 | BIT6 | BIT7;

    /*
     * P1.4 & P4.2 pullup or pulldown enabled for SW1 & SW2
     */
    P1REN = BIT4;
    P4REN = BIT2;

    /*
     * P1.4 & P4.2 pullup selected for SW1 & SW2
     * P2.0 high for SPI_CS
     * ... and all other to 0 (LEDs, etc.)
     */
    P1OUT = BIT4;
    P2OUT = BIT0;
    P3OUT = 0;
    P4OUT = BIT2;

    /*
     * P1.4 & P4.2 IRQ high to low selected for SW1 & SW2
     * P2.1 low to high select for TRF_IRQ
     */
    P1IES = BIT4;
    P4IES = BIT2;
    P2IES = 0;

    /*
     * P1.4 & P4.2 IRQ clear for SW1 & SW2
     */
    P1IFG = 0;
    P4IFG = 0;

    /*
     * P1.4 & P4.2 IRQ enabled for SW1 & SW2
     */
    P1IE = BIT4;
    P4IE = BIT2;

    /*
     * SPI Primary function on P3.2, P3.5 & P3.6
     */
    P3SEL0 = /*BIT0 | BIT1 | */BIT2 | /*BIT3 | BIT4 | */BIT5 | BIT6 /*| BIT7*/;

    /*
     * SPI parameters
     */
    UCB1CTLW0 = UCCKPH_0 | UCCKPL__LOW | UCMSB_1 | UC7BIT__8BIT | UCMST__MASTER | UCMODE_0 | UCSYNC__SYNC | UCSSEL__SMCLK | UCSTEM_0 | UCSWRST;
    UCB1BRW = UCBR2; // UCBR2 = 4, 16000000 / 4 = 4000000
    UCB1CTLW0 &= ~UCSWRST;
}

void TIMER_delay_Milliseconds_internal(uint16_t n_unit_ms) // max is UINT16_MAX ( 1985 ms * 33 = ~ UINT16_MAX )
{
    TA0CCR0 = n_unit_ms;
    g_irq_TA0 = false;
    TA0CTL = TASSEL__ACLK | ID__1 | MC__UP | TACLR | TAIE_1 | TAIFG_0;

    while(!g_irq_TA0)
    {
        __low_power_mode_0();
    }
    TIMER_stop();
}

void TIMER_start_Milliseconds_internal(uint16_t n_unit_ms) // max is UINT16_MAX ( 1985 ms * 33 = ~ UINT16_MAX )
{
    TA0CCR0 = n_unit_ms;
    g_irq_TA0 = false;
    TA0CTL = TASSEL__ACLK | ID__1 | MC__UP | TACLR | TAIE_1 | TAIFG_0;
}

void TIMER_delay_Microseconds_internal(uint16_t n_unit_us) // max is UINT16_MAX ( 32767 us * 2 = ~ UINT16_MAX )
{
    TA0CCR0 = n_unit_us;
    g_irq_TA0 = false;
    TA0CTL = TASSEL__SMCLK | ID__8 | MC__UP | TACLR | TAIE_1 | TAIFG_0;

    while(!g_irq_TA0)
    {
        __low_power_mode_0();
    }
    TIMER_stop();
}

/*
 * TODO: Can be interesting to see a generic function with a bitmask of what is wanted instead of multiple functions
 */
uint8_t IRQ_Wait_for_SW1()
{
    g_irq_SW1 = false;
    while(!g_irq_SW1)
    {
       __low_power_mode_0();
    }
    g_irq_SW1 = false;

    return IRQ_SOURCE_SW1;
}

uint8_t IRQ_Wait_for_SW1_or_SW2()
{
    uint8_t ret = IRQ_SOURCE_NONE;

    g_irq_SW1 = false;
    g_irq_SW2 = false;

    while(!g_irq_SW1 && !g_irq_SW2)
    {
       __low_power_mode_0();
    }

    if(g_irq_SW1)
    {
        g_irq_SW1 = false;
        ret |= IRQ_SOURCE_SW1;
    }

    if(g_irq_SW2)
    {
        g_irq_SW2 = false;
        ret |= IRQ_SOURCE_SW2;
    }

    return ret;
}

uint8_t IRQ_Wait_for_SW1_or_SW2_or_TRF(uint8_t *pTRF7970A_irqStatus)
{
    uint8_t ret = IRQ_SOURCE_NONE;

    g_irq_SW1 = false;
    g_irq_SW2 = false;

    g_irq_TRF = TRF_IRQ_READ();
    while(!g_irq_TRF && !g_irq_SW1 && !g_irq_SW2)
    {
        __low_power_mode_0();
    }

    if(g_irq_TRF)
    {
        *pTRF7970A_irqStatus = TRF7970A_getIrqStatus();
        g_irq_TRF = false;
        ret |= IRQ_SOURCE_TRF7970A;
    }

    if(g_irq_SW1)
    {
        g_irq_SW1 = false;
        ret |= IRQ_SOURCE_SW1;
    }

    if(g_irq_SW2)
    {
        g_irq_SW2 = false;
        ret |= IRQ_SOURCE_SW2;
    }

    return ret;
}

uint8_t IRQ_Wait_for_SW1_or_TRF(uint8_t *pTRF7970A_irqStatus)
{
    uint8_t ret = IRQ_SOURCE_NONE;

    g_irq_SW1 = false;

    g_irq_TRF = TRF_IRQ_READ();
    while(!g_irq_TRF && !g_irq_SW1)
    {
        __low_power_mode_0();
    }

    if(g_irq_TRF)
    {
        *pTRF7970A_irqStatus = TRF7970A_getIrqStatus();
        g_irq_TRF = false;
        ret |= IRQ_SOURCE_TRF7970A;
    }

    if(g_irq_SW1)
    {
        g_irq_SW1 = false;
        ret |= IRQ_SOURCE_SW1;
    }

    return ret;
}


uint8_t IRQ_Wait_for_SW1_or_SW2_or_Timeout(uint16_t timeout_ms)
{
    uint8_t ret = IRQ_SOURCE_NONE;

    g_irq_SW1 = false;
    g_irq_SW2 = false;

    TIMER_start_Milliseconds(timeout_ms);
    while(!g_irq_TA0 && !g_irq_SW1 && !g_irq_SW2)
    {
        __low_power_mode_0();
    }
    TIMER_stop();

    if(g_irq_SW1)
    {
        g_irq_SW1 = false;
        ret |= IRQ_SOURCE_SW1;
    }

    if(g_irq_SW2)
    {
        g_irq_SW2 = false;
        ret |= IRQ_SOURCE_SW2;
    }

    if(g_irq_TA0)
    {
        ret |= IRQ_SOURCE_TIMER;
    }

    return ret;
}

uint8_t IRQ_Wait_for_SW1_or_SW2_or_TRF_or_Timeout(uint8_t *pTRF7970A_irqStatus, uint16_t timeout_ms)
{
    uint8_t ret = IRQ_SOURCE_NONE;

    TIMER_start_Milliseconds(timeout_ms);
    g_irq_TRF = TRF_IRQ_READ();
    while(!g_irq_TRF && !g_irq_TA0 && !g_irq_SW1 && !g_irq_SW2)
    {
        __low_power_mode_0();
    }
    TIMER_stop();

    if(g_irq_TRF)
    {
        *pTRF7970A_irqStatus = TRF7970A_getIrqStatus();
        g_irq_TRF = false;
        ret |= IRQ_SOURCE_TRF7970A;
    }

    if(g_irq_SW1)
    {
        g_irq_SW1 = false;
        ret |= IRQ_SOURCE_SW1;
    }

    if(g_irq_SW2)
    {
        g_irq_SW2 = false;
        ret |= IRQ_SOURCE_SW2;
    }

    if(g_irq_TA0)
    {
        ret |= IRQ_SOURCE_TIMER;
    }

    return ret;
}

/*
 * Interrupt vector for SW1 (P1.4)
 * Usually handled by switch(P1IV) and case P1IV__P1IFG4:, but as there is only one...
 */
#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    g_irq_SW1 = true;
    __low_power_mode_off_on_exit();
    P1IV = 0;
}

/*
 * Interrupt vector for SW2 (P4.2)
 * Usually handled by switch(P4IV) and case P4IV__P4IFG2:, but as there is only one...
 */
#pragma vector=PORT4_VECTOR
__interrupt void Port4_ISR(void)
{
    g_irq_SW2 = true;
    __low_power_mode_off_on_exit();
    P4IV = 0;
}

/*
 * Interrupt vector for TRF_IRQ (P2.1)
 * Usually handled by switch(P2IV) and case P2IV__P2IFG1:, but as there is only one...
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
    g_irq_TRF = true;
    __low_power_mode_off_on_exit();
    P2IV = 0;
}

/*
 * Interrupt vector for Timer TA0
 */
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMERA0_ISR (void)
{
    TA0CTL &= ~TAIFG;
    g_irq_TA0 = true;
    __low_power_mode_off_on_exit();
}
