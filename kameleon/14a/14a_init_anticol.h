/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "14a.h"

#define K14A_ANTICOL_GENERIC_DELAY_CYCLES   290

bool k14a_anticol_loop(); // we try to know by IRQ Status and fifo size before to know if we must enter here
void k14a_anticol_prepare_buffer(const T3A_INFOS *pt3a_infos);
