/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208f30c..0x0208f33c, 48 bytes, 4 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__si_class_type_info 0x0209a764); it stays a number because the tree
 * declares no _ZTV array.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern char _ZTSN5dPa_c7level_c26checkWaterRippleCallback_cE[];
extern u32 _ZTIN5dPa_c7level_c10callback_cE[];
extern char _ZTSN5dPa_c7level_c20checkYoganCallback_cE[];
extern u32 _ZTIN5dPa_c7level_c16simpleCallback_cE[];
extern char _ZTSN5dPa_c7level_c20checkWaterCallback_cE[];
extern char _ZTSN5dPa_c7level_c16bubbleCallback_cE[];

/* 0x0208f324: 12 bytes. */
u32 _ZTIN5dPa_c7level_c20checkWaterCallback_cE[3] = {
    0x0209a764,
    (u32)_ZTSN5dPa_c7level_c20checkWaterCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,
};

/* 0x0208f318: 12 bytes. */
u32 _ZTIN5dPa_c7level_c20checkYoganCallback_cE[3] = {
    0x0209a764,
    (u32)_ZTSN5dPa_c7level_c20checkYoganCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};

/* 0x0208f30c: 12 bytes. */
u32 _ZTIN5dPa_c7level_c26checkWaterRippleCallback_cE[3] = {
    0x0209a764,
    (u32)_ZTSN5dPa_c7level_c26checkWaterRippleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c10callback_cE,
};

/* 0x0208f330: 12 bytes. */
u32 _ZTIN5dPa_c7level_c16bubbleCallback_cE[3] = {
    0x0209a764,
    (u32)_ZTSN5dPa_c7level_c16bubbleCallback_cE,
    (u32)_ZTIN5dPa_c7level_c16simpleCallback_cE,
};
