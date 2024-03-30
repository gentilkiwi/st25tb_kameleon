/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "st25tb.h"
/*
 * SRT512
 *  t0 Antenna reversal delay 128/fS  151 �s
 *  t1 Synchronization delay          151 �s
 *  t2 Answer to new request delay >= 132 �s
 *
 * ST25TB512-AT
 *  t0 Antenna reversal delay 128/fS  159 �s
 *  t1 Synchronization delay          151 �s
 *  t2 Answer to new request delay >= 132 �s
 *
 *  -> focus on t0 < 159 �s ?
 */
#define ST25TB_TARGET_DELAY_US_GLOBAL  81
#define ST25TB_TARGET_DELAY_US_MEDIUM  17
#define ST25TB_TARGET_DELAY_US_SMALL   8

typedef enum __attribute__((__packed__)) _tSt25TbState {
    Invalid,
    PowerOff,
    Ready,
    Inventory,
    Selected,
    Deselected,
    Deactivated,
} tSt25TbState;

void ST25TB_Target_ResetState();
tSt25TbState ST25TB_Target_StateMachine();
