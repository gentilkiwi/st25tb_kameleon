/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "board.h"

/* ST25TB memory support
 * - SLOTS_ST25TB_SUPPORT_4K will enable support for ST25TB04K (and below) - but is 512 bytes per slot (524)
 * - SLOTS_ST25TB_SUPPORT_2K will enable support for ST25TB02K (and below) - but is 256 bytes per slot (268)
 * - otherwise, only ST25TB512 is supported - it is only 64 bytes per slot (76)
 * in all cases, you must add 4 bytes (SYSTEM) + 8 bytes (UID) for each slots
 * default is SLOTS_ST25TB_SUPPORT_4K
 */
#define SLOTS_ST25TB_SUPPORT_4K
//#define SLOTS_ST25TB_SUPPORT_2K
//#define SLOTS_ST25TB_SUPPORT_512

#if defined(SLOTS_ST25TB_SUPPORT_4K)
#define SLOTS_ST25TB_SECTORS_INTERNAL   ((4096 / 8 / 4) + 3)
#elif defined(SLOTS_ST25TB_SUPPORT_2K)
#define SLOTS_ST25TB_SECTORS_INTERNAL   ((2048 / 8 / 4) + 3)
#elif defined(SLOTS_ST25TB_SUPPORT_512)
#define SLOTS_ST25TB_SECTORS_INTERNAL   (( 512 / 8 / 4) + 3)
#else
#error No ST25TB support selected
#endif

/* ST25TB available slots number
 * 8 is recommended, if you want more you'll have to adapt slots.c to avoid empty persistent content
 */
#define SLOTS_ST25TB_COUNT  8

#define SLOTS_ST25TB_INDEX_SYSTEM   (SLOTS_ST25TB_SECTORS_INTERNAL - 3)
#define SLOTS_ST25TB_INDEX_UID      (SLOTS_ST25TB_SECTORS_INTERNAL - 2)
#define SLOTS_ST25TB_INDEX_UID_2    (SLOTS_ST25TB_SECTORS_INTERNAL - 1)


/* SLOTS_ST25TB_PERSISTENT_DATA has attribute PERSISTENT in C file */
extern uint8_t SLOTS_ST25TB_PERSISTENT_DATA[SLOTS_ST25TB_COUNT][SLOTS_ST25TB_SECTORS_INTERNAL][4];
extern uint8_t SLOTS_ST25TB_Current[SLOTS_ST25TB_SECTORS_INTERNAL][4];

void SLOTS_Change(uint8_t index);
void SLOTS_Load(uint8_t index);
void SLOTS_Save(uint8_t index);

#define SLOTS_FIND_INVALID_INDEX    0xff
uint8_t SLOTS_FindByUID(uint8_t pui8Data[8]); // ret == SLOTS_FIND_INVALID_INDEX -> not found

#define SLOTS_Save_Current()        SLOTS_Save(Settings.CurrentSlot)
