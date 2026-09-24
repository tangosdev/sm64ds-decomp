// The card driver's host pieces: the storage its ROM TUs name that nothing
// hosted yet, and the two ARM-only answers they link against.
//
// Run linkfull, lane S4CARD. port/slice_w28_card.txt puts the ROM's own card
// read path and bring-up into walk_window and walk_window_hires (the driver's
// TUs from func_0205db2c down to func_02060a64, plus func_02060890's IRQ arm);
// this file is what those TUs reach that no other file defines.
//
// THE STORAGE, both arm9 .bss (config/arm9/symbols.txt, kind:bss), so zero is
// the cartridge's own start value. Each size is the ROM span to the next
// symbol, not the width one caller touches:
//
//   data_020a8780  0x220  the CPU read path's object. +0x00 the page reader
//                         (src/func_02060890.c stores func_02060a64 there and
//                         src/func_02060a30.c calls through it), +0x04 the
//                         ROMCTRL word src/func_02060b64.c builds, +0x08 the
//                         ROM address of the page cached at +0x20, +0x20 that
//                         page, 0x200 bytes (src/func_02060a64.c, func_02060ebc.c).
//                         0x020a8780 to data_020a89a0.
//   data_020a89a0  4      IRQ::GameCardIREQMCHandler's "fired" word. The next
//                         symbol, data_020a89a4 (the handler's callback), is
//                         hal/boot_globals.cpp's.
//
// Both are DS state a savestate rolls back, so both are in .dsstate -- in the
// suffix $yzzcard, which sorts after every contribution already there ($yzsnd10
// is the last) and before the $zzz high sentinel: the captured span grows at its
// tail and no hosted global moves a byte (hal/snd_globals.cpp's idiom and
// reason; port/tools/battery.py's header says why a move would matter).

#include <stdint.h>

#define W28_CARD_RUN(sec, name, size, algn)                          \
    __pragma(section(sec, read, write))                              \
    extern "C" __declspec(allocate(sec)) __declspec(align(algn))     \
    unsigned char name[size] = {0}

W28_CARD_RUN(".dsstate$yzzcard00", data_020a8780, 0x220, 32);
W28_CARD_RUN(".dsstate$yzzcard01", data_020a89a0, 4, 4);

// The flat name src/func_02061138.c installs as the IRQ-20 handler, onto the
// matched namespace function src/_ZN3IRQ21GameCardIREQMCHandlerEv.cpp defines
// (ntr/runtime.cpp's IRQ rows are the same bridge). A name bridge only: the
// function takes nothing and returns nothing, so there is no receiver to carry.
#pragma comment(linker, "/alternatename:__ZN3IRQ21GameCardIREQMCHandlerEv=?GameCardIREQMCHandler@IRQ@@YAXXZ")

extern "C" {

// PORT_HOST_ABI: hand-asm primitive (src/ carries the banner): `mrc p15, 0,
//   r0, c9, c1, 0` reads the DTCM region register, and no x86 front end
//   assembles a coprocessor read. The answer is the base the ROM's own crt0
//   wrote there: `ldr r0, =0x023c0000` at 0x02004a60 and `mcr p15, 0, r0, c9,
//   c1, 0` at 0x02004a68 (config/arm9/dtcm starts at 0x023c0000), masked to
//   0xfffff000 as the body does. Its one caller here is src/func_02060b64.c's
//   card-DMA arm, which never runs while FS_Init's channel argument is -1.
unsigned int _ZN4CP1518GetDTCMBaseAddressEv(void) { return 0x023c0000u; }

// PORT_HOST_ABI: an ADDRESS, not a body. src/func_02060b64.c compares a
//   transfer's end against `(u32)&func_01ff8000`, the ITCM base (config/arm9/
//   itcm starts at 0x01ff8000 with this symbol), to keep card DMA out of ITCM.
//   The port has no ITCM at that address and the comparison is inside the same
//   card-DMA arm; this symbol exists so the ROM's own TU links as written.
extern const unsigned char func_01ff8000 = 0;

}  // extern "C"
