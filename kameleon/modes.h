/*  Benjamin DELPY `gentilkiwi`
    https://blog.gentilkiwi.com
    benjamin@gentilkiwi.com
    Licence : https://creativecommons.org/licenses/by/4.0/
*/
#pragma once
#include "board.h"

#include "st25tb/st25tb.h"

#include "modes/emulate.h"
#include "modes/rewrite.h"
#include "modes/detect.h"
#include "modes/learn.h"
#include "modes/unk.h"

typedef void (* PMODE_FUNCTION) ();
typedef struct _KAMELEON_MODE {
    const PMODE_FUNCTION current;
    const struct _KAMELEON_MODE *next;
    const uint8_t ledsModesBitmask;
} KAMELEON_MODE, *PKAMELEON_MODE;
