/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "slots.h"

void SLOTS_Change(uint8_t index)
{
    SLOTS_Load(index);
    if(index != Settings.CurrentSlot)
    {
        Settings.CurrentSlot = index;
    }
#if SLOTS_ST25TB_COUNT > 8
    LEDS_SLOTS_Bitmask(index);
#else
    LED_Slot(index);
#endif
}

void SLOTS_Load(uint8_t index)
{
    memcpy(SLOTS_ST25TB_Current, SLOTS_ST25TB_PERSISTENT_DATA[index], sizeof(SLOTS_ST25TB_PERSISTENT_DATA[index]));
}

void SLOTS_Save(uint8_t index)
{
    memcpy(SLOTS_ST25TB_PERSISTENT_DATA[index], SLOTS_ST25TB_Current, sizeof(SLOTS_ST25TB_PERSISTENT_DATA[index]));
}

uint8_t SLOTS_FindByUID(uint8_t pui8Data[8]) // ret == SLOTS_FIND_INVALID_INDEX -> not found
{
    uint8_t ret = SLOTS_FIND_INVALID_INDEX, i;
    for(i = 0; i < SLOTS_ST25TB_COUNT; i++)
    {
        if((*(uint64_t*) pui8Data) == (*(uint64_t*) SLOTS_ST25TB_PERSISTENT_DATA[i][SLOTS_ST25TB_INDEX_UID]))
        {
            ret = i;
            break;
        }
    }

    return ret;
}

uint8_t SLOTS_ST25TB_Current[SLOTS_ST25TB_SECTORS_INTERNAL][4];

#pragma PERSISTENT(SLOTS_ST25TB_PERSISTENT_DATA)
uint8_t SLOTS_ST25TB_PERSISTENT_DATA[SLOTS_ST25TB_COUNT][SLOTS_ST25TB_SECTORS_INTERNAL][4] = {
    {   /* Slot 0 */
        #include "slots_fixed_content.h"
        { 0xff, 0x00, 0x00, 0x00 }, // SLOTS_ST25TB_INDEX_UID // Test with libnfc and ACR122U did not like 0x00 at the end (?)
        { 0x00, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#if SLOTS_ST25TB_COUNT > 1
    {   /* Slot 1 */
        #include "slots_fixed_content.h"
        { 0xff, 0x11, 0x11, 0x11 }, // SLOTS_ST25TB_INDEX_UID
        { 0x11, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 2
    {   /* Slot 2 */
        #include "slots_fixed_content.h"
        { 0xff, 0x22, 0x22, 0x22 }, // SLOTS_ST25TB_INDEX_UID
        { 0x22, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 3
    {   /* Slot 3 */
        #include "slots_fixed_content.h"
        { 0xff, 0x33, 0x33, 0x33 }, // SLOTS_ST25TB_INDEX_UID
        { 0x33, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 4
    {   /* Slot 4 */
        #include "slots_fixed_content.h"
        { 0xff, 0x44, 0x44, 0x44 }, // SLOTS_ST25TB_INDEX_UID
        { 0x44, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 5
    {   /* Slot 5 */
        #include "slots_fixed_content.h"
        { 0xff, 0x55, 0x55, 0x55 }, // SLOTS_ST25TB_INDEX_UID
        { 0x55, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 6
    {   /* Slot 6 */
        #include "slots_fixed_content.h"
        { 0xff, 0x66, 0x66, 0x66 }, // SLOTS_ST25TB_INDEX_UID
        { 0x66, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
#if SLOTS_ST25TB_COUNT > 7
    {   /* Slot 7 */
        #include "slots_fixed_content.h"
        { 0xff, 0x77, 0x77, 0x77 }, // SLOTS_ST25TB_INDEX_UID
        { 0x77, 0x33, 0x02, 0xd0 }, // SLOTS_ST25TB_INDEX_UID_2
    },
#endif
};
