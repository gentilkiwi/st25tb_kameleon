/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "14a.h"

#define NTAG_CMD_GET_VERSION            0x60
#define NTAG_CMD_READ                   0x30 // See rollover :')
#define NTAG_CMD_FAST_READ              0x3a
#define NTAG_CMD_WRITE                  0xa2
#define NTAG_CMD_COMPATIBILITY_WRITE    0xa0
#define NTAG_CMD_READ_CNT               0x39
#define NTAG_CMD_INCR_CNT               0xa5
#define NTAG_CMD_PWD_AUTH               0x1b
#define NTAG_CMD_READ_SIG               0x3c
#define NTAG_CMD_CHECK_TEARING_EVENT    0x3e
#define NTAG_CMD_VCSL                   0x4b

/*
 * Here not dealing with write/status/etc.
 * 4 bits emulation not available on the TRF (?)
 */
extern const T3A_INFOS t3a_infos_ntag210;
void Dispatch_14a_NTAG210(const uint8_t **ppcbData, uint8_t *pcbData);
