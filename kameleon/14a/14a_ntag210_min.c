/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "14a_ntag210_min.h"
#if defined(ST25TB_SUPPORT_A_NTAG210)
const T3A_INFOS t3a_infos_ntag210 = {
    .UID = {0x04, 0xa8, 0xc4, 0xaa, 0x28, 0x63, 0x80}, .tUID = T3A_UID_7BYTES,
    .ATQA = {0x44, 0x00}, .SAK = 0x00,
    .ATS = {0x00}, .cbATS = 0,
};

const uint8_t NTAG210_DATA[][4] = {
    /* 0x00 */ {0x04, 0xa8, 0xc4, 0xe0}, // UID0
    /* 0x01 */ {0xaa, 0x28, 0x63, 0x80}, // UID1
    /* 0x02 */ {0x61, 0x48, 0xff, 0xff}, // static lock bytes
    /* 0x03 */ {0xe1, 0x10, 0x06, 0x00}, // Capability Container (CC)
    /* 0x04 */ {0x03, 0x15, 0xd1, 0x01}, // User Memory | begin
    /* 0x05 */ {0x11, 0x54, 0x02, 0x66}, // .
    /* 0x06 */ {0x72, 0xf0, 0x9f, 0xa5}, // .
    /* 0x07 */ {0x9d, 0xe2, 0x9d, 0xa4}, // .
    /* 0x08 */ {0xef, 0xb8, 0x8f, 0xf0}, // .
    /* 0x09 */ {0x9f, 0xa6, 0x86, 0xfe}, // .
    /* 0x0A */ {0x00, 0x00, 0x00, 0x00}, // .
    /* 0x0B */ {0x00, 0x00, 0x00, 0x00}, // .
    /* 0x0C */ {0x00, 0x00, 0x00, 0x00}, // .
    /* 0x0D */ {0x00, 0x00, 0x00, 0x00}, // .
    /* 0x0E */ {0x00, 0x00, 0x00, 0x00}, // .
    /* 0x0F */ {0x00, 0x00, 0x00, 0x00}, // User memory | end
    /* 0x10 */ {0x04, 0x00, 0x00, 0xff}, // CFG0
    /* 0x11 */ {0x40, 0x05, 0x00, 0x00}, // CFG1 - CFGLCK: Write locking bit for the user configuration - user configuration permanently locked against write access
    /* 0x12 */ {0xff, 0xff, 0xff, 0xff}, // PWD
    /* 0x13 */ {0x00, 0x00, 0x00, 0x00}, // PACK, RFUI
};
const uint8_t NTAG210_VERSION[] = {0x00, 0x04, 0x04, 0x01, 0x01, 0x00, 0x0b, 0x03};
const uint8_t NTAG210_SIGNATURE[] = {0xd3, 0xae, 0x20, 0x27, 0x08, 0xb2, 0xcc, 0x51, 0x76, 0x4e, 0xa2, 0x89, 0x62, 0xae, 0xc9, 0xb8, 0xc8, 0xd0, 0x80, 0xbc, 0xad, 0xb1, 0x20, 0xd3, 0x84, 0xfa, 0xa3, 0xbc, 0xe5, 0xff, 0xa6, 0x1e};

void Dispatch_14a_NTAG210(const uint8_t **ppcbData, uint8_t *pcbData)
{
    uint8_t addr;
    uint32_t aligned;

    switch(g_ui8_14a_Buffer[0])
    {
    case NTAG_CMD_GET_VERSION:
        if(g_ui8_cb14a_Buffer == 1)
        {
            *ppcbData = NTAG210_VERSION;
            *pcbData = sizeof(NTAG210_VERSION);
        }

        break;

    case NTAG_CMD_READ:
        if(g_ui8_cb14a_Buffer == 2)
        {
            addr = g_ui8_14a_Buffer[1];
            if(addr < (sizeof(NTAG210_DATA) / sizeof(NTAG210_DATA[0])))
            {
                *ppcbData = NTAG210_DATA[addr];
                *pcbData = 16; // ouch, TODO: FIX rollover, or not.... I pushed data after DATA
            }
        }

        break;

    case NTAG_CMD_PWD_AUTH:
        if(g_ui8_cb14a_Buffer == 1 + sizeof(NTAG210_DATA[0x12]))
        {
            /* Something something something... align! */
            aligned = ((uint32_t ) g_ui8_14a_Buffer[4] << 24) | ((uint32_t ) g_ui8_14a_Buffer[3] << 16) | ((uint32_t ) g_ui8_14a_Buffer[2] << 8) | (uint32_t ) g_ui8_14a_Buffer[1];
            if(*(uint32_t *)(NTAG210_DATA[0x12]) == aligned)
            {
                *ppcbData = NTAG210_DATA[0x13];
                *pcbData = 2;
            }
        }

        break;

    case NTAG_CMD_READ_SIG:
        if(g_ui8_cb14a_Buffer == 2)
        {
            *ppcbData = NTAG210_SIGNATURE;
            *pcbData = sizeof(NTAG210_SIGNATURE);
        }

        break;

    default:
        ;
    }
}
#endif
