/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "board.h"

#define SLOTS_COUNT 8

#define SLOTS_ST25TB_INDEX_SYSTEM   0x80
#define SLOTS_ST25TB_INDEX_UID      0x81
#define SLOTS_ST25TB_INDEX_UID_2    0x82

/* SLOTS_ST25TB_PERSISTENT_DATA has attribute PERSISTENT in C file */
extern uint8_t SLOTS_ST25TB_PERSISTENT_DATA[SLOTS_COUNT][0x80 + 1 + 2][4];
extern uint8_t SLOTS_ST25TB_Current[0x80 + 1 + 2][4];

void SLOTS_Change(uint8_t index);
void SLOTS_Load(uint8_t index);
void SLOTS_Save(uint8_t index);

#define SLOTS_FIND_INVALID_INDEX    0xff
uint8_t SLOTS_FindByUID(uint8_t pui8Data[8]); // ret == SLOTS_FIND_INVALID_INDEX -> not found

#define SLOTS_Save_Current()        SLOTS_Save(Settings.CurrentSlot)
