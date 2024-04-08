/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "14a.h"

/*
 * Bit-number is different from ISO
 * we start at 0, like civilized people
 */
#define BLOCK_PCB_TYPE_I_MASK           0b11100010
#define BLOCK_PCB_TYPE_I_VALUE          0b00000010

#define BLOCK_PCB_TYPE_R_MASK           0b11100010
#define BLOCK_PCB_TYPE_R_VALUE          0b10100010

#define BLOCK_PCB_TYPE_S_MASK           0b11000111
#define BLOCK_PCB_TYPE_S_VALUE          0b11000010

/*
 * Coding of I-block PCB (BLOCK_PCB_TYPE_I)
 */
#define BLOCK_PCB_I_CHAINING_MASK       0b00010000 // B4
#define BLOCK_PCB_I_CHAINING_NO         0b00000000 // B4
#define BLOCK_PCB_I_CHAINING            0b00010000 // B4

#define BLOCK_PCB_I_CID_FOLLOWING_MASK  0b00001000 // B3
#define BLOCK_PCB_I_CID_FOLLOWING_NO    0b00000000 // B3
#define BLOCK_PCB_I_CID_FOLLOWING       0b00001000 // B3

#define BLOCK_PCB_I_NAD_FOLLOWING_MASK  0b00000100 // B2
#define BLOCK_PCB_I_NAD_FOLLOWING_NO    0b00000000 // B2
#define BLOCK_PCB_I_NAD_FOLLOWING       0b00000100 // B2

#define BLOCK_PCB_I_BLOCK_NUMBER_MASK   0b00000001 // B0

/*
 * Coding of R-block PCB (BLOCK_PCB_TYPE_R)
 */
#define BLOCK_PCB_R_ACK_NACK_MASK       0b00010000 // B4
#define BLOCK_PCB_R_ACK_NACK_VAL_NACK   0b00010000 // B4
#define BLOCK_PCB_R_ACK_NACK_VAL_ACK    0b00000000 // B4

#define BLOCK_PCB_R_CID_FOLLOWING_MASK  0b00001000 // B3
#define BLOCK_PCB_R_CID_FOLLOWING_NO    0b00000000 // B3
#define BLOCK_PCB_R_CID_FOLLOWING       0b00001000 // B3

#define BLOCK_PCB_R_BLOCK_NUMBER_MASK   0b00000001 // B0

/*
 * Coding of S-block PCB (BLOCK_PCB_TYPE_S)
 */
#define BLOCK_PCB_S_CMD_MASK            0b00110000 // B5-B4
#define BLOCK_PCB_S_CID_FOLLOWING_MASK  0b00001000 // B3


extern const T3A_INFOS t3a_infos_st25ta512;
void Dispatch_14a_ST25TA512(const uint8_t **ppcbData, uint8_t *pcbData);
