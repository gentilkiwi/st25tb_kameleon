/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "../board.h"

/** 'A' settings
 ** at the beginning, ST25TB kameleon was only to emulate/play with ST25TB technology
 ** but at one point, I wanted to play with 'A' emulation and the TRF (spoiler alert: it sucks - do not enable or use it)
 **/
#define ST25TB_SUPPORT_A_NTAG210
#define ST25TB_SUPPORT_A_ST25TA512

typedef void (* const PDISPATCH_14A_ROUTINE) (const uint8_t **ppcbData, uint8_t *pcbData);

typedef enum __attribute__((__packed__)) _t3UIDType {
    T3A_UID_4BYTES = 4,
    T3A_UID_7BYTES = 7,
    T3A_UID_10BYTES = 10,
} t3UIDType;

typedef struct _T3A_INFOS {
    uint8_t UID[T3A_UID_10BYTES];
    t3UIDType tUID;
    uint8_t ATQA[2];
    uint8_t SAK;
    uint8_t ATS[0x20];
    uint8_t cbATS;
} T3A_INFOS;

typedef enum __attribute__((__packed__)) _t3RecvType {
    T3A_RECV_NONE = 0,
    T3A_RECV_INIT = 1,
    T3A_RECV_DATA = 2,
} t3RecvType;

#define ALL_REQ     0x52 // WUPA
#define SENS_REQ    0x26 // REQA
// SDD_REQ: SEL_CMD - SEL_PAR - data bit 1 ... data bit n
#define SDD_REQ_CL1 0x93
#define SDD_REQ_CL2 0x95
#define SDD_REQ_CL3 0x97

// SEL_REQ: SEL_CMD - 70h - NFCID1 CLn - BCC
#define HLTA    0x50 // 0x00
#define RATS    0xe0 // 0x..

#define CASCADE_TAG 0x88

void k14a_TRF7970A_Mode();
t3RecvType k14a_Recv(uint8_t irqProvided);
bool k14a_Send(const uint8_t *pcbData, const uint8_t cbData, const uint8_t bIsCRC);
bool k14a_Dispatch_14a(const T3A_INFOS *pt3a_infos, PDISPATCH_14A_ROUTINE pDispatcher);

extern uint8_t g_ui8_14a_Buffer[0x7f];
extern uint8_t g_ui8_cb14a_Buffer;

#include "14a_init_anticol.h"

