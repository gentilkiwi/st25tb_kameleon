# st25tb_kameleon

ST25TB / SRx NFC Emulator / Initiator - kameleon - based on TI TRF7970A with MSP430

This is an **additionnal** support project of `ST25TB series NFC tags for fun in French public transports` [ST25TB_transport.pdf](https://raw.githubusercontent.com/gentilkiwi/st25tb_kiemul/main/ST25TB_transport.pdf) - it is based on first [st25tb_kiemul](https://github.com/gentilkiwi/st25tb_kiemul) project.

## Project

`st25tb_kameleon` project is ready for Texas Intruments Code Composer Studio (CCS) IDE 12+, https://www.ti.com/tool/CCSTUDIO. You can use the local IDE or the online one (CCS Cloud).

This version of the project does **not** use [`Driverlib`](https://www.ti.com/tool/MSPDRIVERLIB) and is slightly faster/optimized.

As we have a _gigantic_ amount of 16KB of FRAM with the `MSP430FR2673` MCU, project is optimized for speed (`--opt_for_speed=5`), and use more space than usually (< 12KB, but considering it supports up to 8 `ST25TB04K` cards - with 4KB of persistent storage - it's not as bad).


## Flash

You can program your board from the IDE itself, but `.hex` files produced/downloaded can also be used with `MSP430Flasher` (https://www.ti.com/tool/MSP430-FLASHER) or `UniFlash` (https://www.ti.com/tool/UNIFLASH).

[`MSP-FET`](https://www.ti.com/tool/MSP-FET) is required to flash the board, but you can also use the embedded `eZ-FET lite` from another Launchpad card you already have :)

### Example:

- `MSP430Flasher -i TIUSB -n MSP430FR2673 -z [VCC,RESET] -e ERASE_ALL -v -w st25tb_kameleon.hex`

```
> MSP430Flasher -i TIUSB -n MSP430FR2673 -z [VCC,RESET] -e ERASE_ALL -v -w st25tb_kameleon.hex

* -----/|-------------------------------------------------------------------- *
*     / |__                                                                   *
*    /_   /   MSP Flasher v1.3.20                                             *
*      | /                                                                    *
* -----|/-------------------------------------------------------------------- *
*
* Evaluating triggers...done
* Checking for available FET debuggers:
* Found USB FET @ COM27 <- Selected
* Initializing interface @ COM27...done
* Checking firmware compatibility:
* FET firmware is up to date.
* Reading FW version...done
* Setting VCC to 3000 mV...done
* Accessing device...done
* Reading device information...done
* Loading file into device...done
* Verifying memory (st25tb_kameleon.hex)...done
*
* ----------------------------------------------------------------------------
* Arguments   : -i TIUSB -n MSP430FR2673 -z [VCC,RESET] -e ERASE_ALL -v -w st25tb_kameleon.hex
* ----------------------------------------------------------------------------
* Driver      : loaded
* Dll Version : 31501001
* FwVersion   : 31200000
* Interface   : TIUSB
* HwVersion   : U 3.0
* JTAG Mode   : AUTO
* Device      : MSP430FR2673
* EEM         : Level 5, ClockCntrl 2
* Erase Mode  : ERASE_ALL
* Prog.File   : st25tb_kameleon.hex
* Verified    : TRUE
* BSL Unlock  : FALSE
* InfoA Access: FALSE
* VCC ON      : 3000 mV
* ----------------------------------------------------------------------------
* Resetting device (RST/NMI)...done
* Starting target code execution...done
* Disconnecting from device...done
*
* ----------------------------------------------------------------------------
* Driver      : closed (No error)
* ----------------------------------------------------------------------------
*/
```

#### Notes:
- replace `-i TIUSB` by `-i COMnumber` when dealing with multiple boards connected at the same time ;
- do not hesitate to make another reset by the button after flashing ;
- latest version of `MSP430Flasher` (1.3.20) includes `MSP430.dll` version 3.14, **not supporting `MSP430FR2673`**. Please program from IDE / `UniFlash`, or by copying `MSP430.dll` from [MSPDS](https://www.ti.com/tool/MSPDS) (version 3.15.1.1 at this time)


## How to use it?
_basics_

At startup/reset, the content of the last used slot is loaded from the flash memory. If no cards were previously learned, a default one at slot `0` with the UID: `d0023300000000ff` is loaded.  
8 slots are available, the selected one is indicated by a blue LED at the bottom of the board.

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|🔵|⚫|⚫|⚫|⚫|⚫|⚫|⚫|

To avoid misusage, at this time, only the `LEARN` mode (with `SLOT/ACT` button) or the `DETECT` mode (by detecting an existing stored card) is able to change the selected slot.  


### Emulator mode - _(default)_
⚪ EMUL - ⚫ WRITE - ⚫ DETECT - ⚫ LEARN - ⚫ (⊙_⊙)?

In this mode, it reacts like a normal SRI/SRT/ST25TB card, with few differences:
- Sectors `0x05` and `0x06` are not limited to decrement ;
- Sector `0xff` (system area / internal is `0x80`) is not used to lock sectors/OTP ;
- Sectors `0x81` and `0x82` can be used to read **and write** UID ;
- Any writing operation to sector `0xfe` will write current emulated card into flash memory, in the current slot.

_basically, it reacts like a normal card but without limitation_

Status LEDs: ❌, ✅ & ● : ⚫ - _not used_

Buttons:
- 🔳 MODE: will go to the next configured mode (`WRITE`)
- 🔳 SLOT/ACT: _not used_


### Writer mode
⚫ EMUL - ⚪ WRITE - ⚫ DETECT - ⚫ LEARN - ⚫ (⊙_⊙)?

In this mode, it will try to write the emulator content back to the original card.  
It starts by searching to detect a card with status led `●` blinking (🔵/⚫).

Status LEDs:
- ❌: 🔴 - indicates that an error occured (not the good UID or write then read error)
  - **does not stop the routine** and will retry until success (or button)
- ✅: 🟢 - indicates that card successfully written (and confirmed) 

Buttons:
- 🔳 MODE: will go to the next configured mode (`DETECT`)
- 🔳 SLOT/ACT: will restart the write loop


### Detect mode
⚫ EMUL - ⚫ WRITE - ⚪ DETECT - ⚫ LEARN - ⚫ (⊙_⊙)?

In this mode, it will try to detect any SRI/SRT/ST25TB card, and search in slots memory any cards with the same UID.  
**If a slot matches, then it is loaded**.  
It starts by searching to detect a card with status led `●` blinking (🔵/⚫), and clearing slots LEDs.

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|⚫|⚫|⚫|⚫|⚫|⚫|⚫|⚫|

Status LEDs:
- ✅: 🟢 - indicates that the card is detected as a SRI/SRT/ST25TB
- ● : 🔵 - indicates that the card is matching with the UID of one existing slot (see bellow for example on slot #4)

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|⚫|⚫|⚫|⚫|🔵|⚫|⚫|⚫|

Buttons:
- 🔳 MODE: will go to the next configured mode (`LEARN`)
- 🔳 SLOT/ACT: will restart the detect loop


### Learn mode - _dangerous!_
⚫ EMUL - ⚫ WRITE - ⚫ DETECT - ⚪ LEARN - ⚫ (⊙_⊙)?

In this mode, it will read an original card and store it into the current slot's persistent memory.  
This mode is also here to change the current slot to another one - to emulate in `EMUL` mode, or to store card into.  
It starts by searching to detect a card with status led `●` blinking (🔵/⚫).

Status LEDs:
- ❌: 🔴 - indicates that an error occured (no confirmed read by example)
  - **does not stop the routine** and will retry until success (or button)
- ✅: 🟢 - indicates that card successfully readed **and saved to the current slot**

Buttons:
- 🔳 MODE: will go to the next configured mode (`(⊙_⊙)?`)
- 🔳 SLOT/ACT: will select and load the next slot
  - no need to read a card if only changing slot is needed


### (⊙_⊙)? mode
⚫ EMUL - ⚫ WRITE - ⚫ DETECT - ⚫ LEARN - ⚪ (⊙_⊙)?

_This mode is not really needed..._ at this time it only displays the first byte in `Counter1` sector.  
It starts by searching to detect a card with status led `●` blinking (🔵/⚫), and clearing slots LEDs.

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|⚫|⚫|⚫|⚫|⚫|⚫|⚫|⚫|

Status LEDs:
- ❌: 🔴 - indicates that the value is 0
  - Slot 0 LED is also set

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|🔵|⚫|⚫|⚫|⚫|⚫|⚫|⚫|

- ✅: 🟢 - indicates that the value is < 8
  - Slot _value_ LED is also set, here for 5:

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|⚫|⚫|⚫|⚫|⚫|🔵|⚫|⚫|

- ● : 🔵 - indicates that the value is >= 8
  - Slots LEDs are selected according to bitmask of the value (up to 255/`0xff`), here for 70:

|0|1|2|3|4|5|6|7|
|-|-|-|-|-|-|-|-|
|⚫|🔵|🔵|⚫|⚫|⚫|🔵|⚫|


## Remarks

- Yep, I recreated a ~2% `Chameleon` like stuff :')
- Yep, I'm happy with it: I do it for myself to learn (& for fun)
- No, I'm not confident to be able to help to add 14B(') to https://github.com/emsec/ChameleonMini/tree/master/Firmware/Chameleon-Mini/Codec
  - otherwise I would have done it
- Yes, you can use other (better) MCU with the `TRF7970A`, even with the original `DLP-7970ABP` board
  - think aout STM32, Raspberry PI Pico or your prefered one with `SPI` support, **really**


## Hardware references

- `MSP430FR2673` - https://www.ti.com/product/MSP430FR2673
- `TRF7970A` - https://www.ti.com/product/TRF7970A


## Kiwi board

To practice, for POC, and for the lulz, a mini-board is also/will be available: https://oshwlab.com/gentilkiwi/st25tb_kameleon


## Question & Answers

### 1. Why a new board?

The main idea of `st25tb_kiemul` project was to be 100% compatible with official TI's Launchpads `MSP-EXP430F5529LP` & `LP-MSP430FR2476`. Firmware (binary produced) for the `LP-MSP430FR2476` Launchpad can be used as-is for my little `st25tb_kiemul` board.  
I made it, and I'm now happy to be able to test/debug what I do on an official Launchpad platform before having problems on my board.  
But for expanding functionnalities (more than 1 slot, more modes, more status, USB powered / LIR2032 battery charging...), a new card had to be created. And to be honest, it makes me practice to learn new (basic) things.

### 2. Why another MCU?

Technically, I did not **need** to change from the `MSP430FR2476TRHAR` used with `st25tb_kiemul` board... 64KB of FRAM & 8KB of SRAM, with a lots of I/O is **largely** enough.  
It was _necessary_ to use another one: no real stock was available for `MSP430FR2476*` (or very little one, and expensive...).  
I choose `MSP430FR2673TRHBR` - < $2.5, more than 1000 in stock - VQFN format, 16KB of FRAM & 4KB of SRAM, with enough IO... promising ! I also discovered later that it was from the very close family of `MSP430FR2476`.

### 3. Why `kameleon`?

The name is more a tribute to original `Chameleon` device & research (@emsec), than because the device is a real `Chameleon`... :')
- https://github.com/emsec/ChameleonMini
- https://shop.kasper.it/chameleonmini/
- https://www.kickstarter.com/projects/1980078555/chameleonmini-a-versatile-nfc-card-emulator-and-mo

Not sure to see original `Chameleon`s or new Ultra branch compatible with `B'` emulation...

### 4. Can I buy one?

Nop, but as firmware is available here, and all is/will be ready on https://oshwlab.com/gentilkiwi/st25tb_kameleon for the hardware part... you'll certainly have it at better price than I did :)

## Author

Benjamin DELPY 🥝 `gentilkiwi`, you can contact me on Twitter ( @gentilkiwi ) or by mail ( benjamin [at] gentilkiwi.com )

This is a personal development, please respect its philosophy and don't use it for bad things!


## Licence

CC BY 4.0 licence - [https://creativecommons.org/licenses/by/4.0/](https://creativecommons.org/licenses/by/4.0/)