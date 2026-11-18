/* C++ runtime handler slot.
 * _ZSt8thandler (std::thandler) holds the code address of _ZSt9dthandlerv
 * (std::dthandler, arm9 0x02073220).
 * ROM 0x0209a6fc..0x0209a700, 4 bytes, 1 configured symbol.
 *
 * Order: mwcc emits one .data section per initialized object and the linker
 * concatenates them in declaration order, so the definitions below are in the
 * order that yields the ascending addresses above.
 */
#include "types.h"

extern void _ZSt9dthandlerv();

/* 0x0209a6fc: 4 bytes. */
u32 _ZSt8thandler[1] = {
    (u32)&_ZSt9dthandlerv,
};
