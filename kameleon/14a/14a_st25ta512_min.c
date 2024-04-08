/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "14a_st25ta512_min.h"

const T3A_INFOS t3a_infos_st25ta512 = {
    .UID = {0x02, 0xe5, 0x00, 0x11, 0x22, 0x33, 0x44}, .tUID = T3A_UID_7BYTES,
    .ATQA = {0x42, 0x00}, .SAK = 0x20,
    .ATS = {0x02, 0x05}, .cbATS = 2, // minimal one instead of .ATS = {0x05, 0x75, 0x80, 0x60, 0x02}, .cbATS = 5
};

const uint8_t NDEF_AID[] = {0xd2, 0x76, 0x00, 0x00, 0x85, 0x01, 0x01};
const uint8_t NDEF_FID[2] = {0x00, 0x01};
const uint8_t NDEF_FILE_DATA[] = {
    0x00, 0x2a,
    0xd1, 0x01, 0x26, 0x55, 0x04, 0x67, 0x69, 0x74, 0x68, 0x75, 0x62, 0x2E, 0x63, 0x6F, 0x6D, 0x2F,
    0x67, 0x65, 0x6E, 0x74, 0x69, 0x6C, 0x6B, 0x69, 0x77, 0x69, 0x2F, 0x73, 0x74, 0x32, 0x35, 0x74,
    0x62, 0x5F, 0x6B, 0x61, 0x6D, 0x65, 0x6C, 0x65, 0x6F, 0x6E
};
uint8_t NDEF_FID_SYSTEM[2] = {0xe1, 0x01};

const uint8_t NDEF_FID_CC[2] = {0xe1, 0x03};
const uint8_t NDEF_CC_FILE_DATA[] = {
    0x00, 0x0f, // CCLEN - Indicates the size of this CC (including this field)
    0x20,       // T4T_VNo - Mapping Version 2.0 (with the Standard Data Structure)
    0x00, 0xff, // MLe (bytes); Maximum R-APDU data size
    0x00, 0x36, // MLc (bytes); Maximum C-APDU data size
    /*
     * NDEF-File_Ctrl_TLV
     */
    0x04,       // Indicates the NDEF-File_Ctrl_TLV
    0x06,       // The length of the V-field is 6 bytes
    0x00, 0x01, // File identifier of the NDEF File - 0001h
    0x00, sizeof(NDEF_FILE_DATA), // We try to be a XX bytes tag
    0x00,       // READ access granted without any security
    0xff,       // No WRITE access granted at all (i.e., Read only)
};


/*
 * Ok, we mix a little bit generic 14443-4 / ISO-DEP / and NDEF here
 */
uint8_t ui8_14ASendBuffer[64];
uint8_t ui8_cb14ASendBuffer;
const uint8_t *pFile;
uint8_t cbFile;

/*
 * Dispatch_14a_ST25TA512 is more a generic NDEF T4 dispatcher
 * This function is not correct at all, not safe ISO DEP, etc.
 * This is just for the fun
 */
void Dispatch_14a_ST25TA512(const uint8_t **ppcbData, uint8_t *pcbData)
{
    uint8_t PCB, CLA, INS, P1, P2, LC;
    uint16_t offset;

    PCB = g_ui8_14a_Buffer[0];
    if((PCB & BLOCK_PCB_TYPE_I_MASK) == BLOCK_PCB_TYPE_I_VALUE)
    {
        ui8_14ASendBuffer[0] = BLOCK_PCB_TYPE_I_VALUE | BLOCK_PCB_I_CHAINING_NO | BLOCK_PCB_I_CID_FOLLOWING_NO | BLOCK_PCB_I_NAD_FOLLOWING_NO | (PCB & BLOCK_PCB_I_BLOCK_NUMBER_MASK);

        if(g_ui8_cb14a_Buffer >= (1 + 5))
        {
            CLA = g_ui8_14a_Buffer[1];
            INS = g_ui8_14a_Buffer[2];
            P1 = g_ui8_14a_Buffer[3];
            P2 = g_ui8_14a_Buffer[4];
            LC = g_ui8_14a_Buffer[5];

            ui8_cb14ASendBuffer = 3;

            switch(CLA)
            {
            case 0x00:

                switch(INS)
                {
                case 0xa4: // SELECT

                     pFile = 0;
                     cbFile = 0;

                     if((P1 == 0x04) && (P2 == 0x00))
                     {
                         if(LC == sizeof(NDEF_AID))
                         {
                             if(!memcmp(NDEF_AID, g_ui8_14a_Buffer + 6, sizeof(NDEF_AID)))
                             {
                                 ui8_14ASendBuffer[1] = 0x90;
                                 ui8_14ASendBuffer[2] = 0x00;
                             }
                             else
                             {
                                 ui8_14ASendBuffer[1] = 0x6A; // FILE_NOT_FOUND
                                 ui8_14ASendBuffer[2] = 0x82;
                             }
                         }
                         else
                         {
                             ui8_14ASendBuffer[1] = 0x67; // WRONG_LENGTH
                             ui8_14ASendBuffer[2] = 0x00;
                         }
                     }
                     else if((P1 == 0x00) && (P2 == 0x0c))
                     {
                         if(LC == 2)
                         {
                             if(!memcmp(NDEF_FID_CC, g_ui8_14a_Buffer + 6, 2)) // TODO: simplify
                             {
                                 pFile = NDEF_CC_FILE_DATA;
                                 cbFile = sizeof(NDEF_CC_FILE_DATA);

                                 ui8_14ASendBuffer[1] = 0x90;
                                 ui8_14ASendBuffer[2] = 0x00;
                             }
                             else if(!memcmp(NDEF_FID, g_ui8_14a_Buffer + 6, 2)) // TODO: simplify
                             {
                                 pFile = NDEF_FILE_DATA;
                                 cbFile = sizeof(NDEF_FILE_DATA);

                                 ui8_14ASendBuffer[1] = 0x90;
                                 ui8_14ASendBuffer[2] = 0x00;
                             }
                             else
                             {
                                 ui8_14ASendBuffer[1] = 0x6a; // FILE_NOT_FOUND
                                 ui8_14ASendBuffer[2] = 0x82;
                             }
                         }
                         else
                         {
                             ui8_14ASendBuffer[1] = 0x67; // WRONG_LENGTH
                             ui8_14ASendBuffer[2] = 0x00;
                         }
                     }
                     else
                     {
                         ui8_14ASendBuffer[1] = 0x6b; // WRONG_P1P2
                         ui8_14ASendBuffer[2] = 0x00;
                     }

                     break;

                 case 0xb0: // READ_BINARY
                     if(pFile && cbFile)
                     {
                         offset = (P1 << 8) | P2;
                         if((offset + LC) <= ((uint16_t) cbFile)) // we know LE = LC here
                         {
                             memcpy(ui8_14ASendBuffer + 1, pFile + offset, LC);
                             ui8_14ASendBuffer[1 + LC] = 0x90;
                             ui8_14ASendBuffer[1 + LC + 1] = 0x00;
                             ui8_cb14ASendBuffer += LC;
                         }
                         else
                         {
                             ui8_14ASendBuffer[1] = 0x67; // WRONG_LENGTH
                             ui8_14ASendBuffer[2] = 0x00;
                         }
                     }
                     else
                     {
                         ui8_14ASendBuffer[1] = 0x6a; // FILE_NOT_FOUND
                         ui8_14ASendBuffer[2] = 0x82;
                     }

                     break;

                 default:
                     ui8_14ASendBuffer[1] = 0x6d; // INS_NOT_SUPPORTED
                     ui8_14ASendBuffer[2] = 0x00;
                 }

                break;

            default:
                ui8_14ASendBuffer[1] = 0x6e; // CLA_NOT_SUPPORTED
                ui8_14ASendBuffer[2] = 0x00;
            }

            *ppcbData = ui8_14ASendBuffer;
            *pcbData = ui8_cb14ASendBuffer;
        }
    }
    else if((PCB & BLOCK_PCB_TYPE_R_MASK) == BLOCK_PCB_TYPE_R_VALUE)
    {
        if((PCB & BLOCK_PCB_R_ACK_NACK_MASK) == BLOCK_PCB_R_ACK_NACK_VAL_NACK)
        {
            ui8_14ASendBuffer[0] = BLOCK_PCB_TYPE_R_VALUE | BLOCK_PCB_R_ACK_NACK_VAL_ACK | BLOCK_PCB_R_CID_FOLLOWING_NO | (PCB & BLOCK_PCB_R_BLOCK_NUMBER_MASK);
            ui8_14ASendBuffer[0] ^= BLOCK_PCB_R_BLOCK_NUMBER_MASK;
            ui8_cb14ASendBuffer = 1;

            *ppcbData = ui8_14ASendBuffer;
            *pcbData = ui8_cb14ASendBuffer;
        }

    }
    else if((PCB & BLOCK_PCB_TYPE_S_MASK) == BLOCK_PCB_TYPE_S_VALUE)
    {
        ui8_14ASendBuffer[0] = PCB;
        ui8_cb14ASendBuffer = 1;

        *ppcbData = ui8_14ASendBuffer;
        *pcbData = ui8_cb14ASendBuffer;
    }
}

