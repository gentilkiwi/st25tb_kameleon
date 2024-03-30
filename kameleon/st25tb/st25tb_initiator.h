/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "st25tb.h"

#define ST25TB_INITIATOR_TIMEOUT_INITIATE           20
#define ST25TB_INITIATOR_TIMEOUT_SELECT             10
#define ST25TB_INITIATOR_TIMEOUT_GENERIC            5
#define ST25TB_INITIATOR_DELAY_BEFORE_RETRY         250
#define ST25TB_INITIATOR_DELAY_WRITE_TIME_OTP       3
#define ST25TB_INITIATOR_DELAY_WRITE_TIME_EEPROM    5
#define ST25TB_INITIATOR_DELAY_WRITE_TIME_COUNTER   7

uint8_t ST25TB_Initiator_Initiate_Select_UID(uint8_t pui8Data[8]);
uint8_t ST25TB_Initiator_Initiate_Select_Block(uint8_t index, uint8_t pui8Data[4]);

uint8_t ST25TB_Initiator_Read_Card();
uint8_t ST25TB_Initiator_Write_Card();
