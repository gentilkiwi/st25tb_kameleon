/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "references.h"

const uint8_t REFERENCES_ST25TB[][REFERENCES_ST25TB_SECTORS_INTERNAL][4] = {
#if defined(REFERENCES_DATA_READY)
#include "references.data"
#endif
};

uint8_t REFERENCES_FindAndLoadByUID(uint8_t pui8Data[8])
{
#if !defined(REFERENCES_DATA_READY)
    (void) pui8Data;
#else
    uint8_t i;

    for (i = 0; i < count_of(REFERENCES_ST25TB); i++)
    {
        if ((*(uint64_t*) pui8Data) == (*(uint64_t*) REFERENCES_ST25TB[i][REFERENCES_ST25TB_INDEX_UID]))
        {
#if (REFERENCES_ST25TB_SECTORS_INTERNAL == SLOTS_ST25TB_SECTORS_INTERNAL)
            memcpy(SLOTS_ST25TB_Current, REFERENCES_ST25TB[i], sizeof(REFERENCES_ST25TB[i]));
#elif (REFERENCES_ST25TB_SECTORS_INTERNAL < SLOTS_ST25TB_SECTORS_INTERNAL)
            memcpy(SLOTS_ST25TB_Current, REFERENCES_ST25TB[i], sizeof(REFERENCES_ST25TB[i]) - (3 * 4));
            memcpy(SLOTS_ST25TB_Current + SLOTS_ST25TB_INDEX_SYSTEM, REFERENCES_ST25TB[i] + REFERENCES_ST25TB_INDEX_SYSTEM, 3 * 4);
#else
#error REFERENCE size is > SLOT size
#endif
            return 1;
        }
    }
#endif
    return 0;
}
