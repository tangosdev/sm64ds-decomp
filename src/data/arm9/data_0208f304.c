/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208f304..0x0208f30c, 8 bytes, 1 configured symbols.
 * A _ZTI record's first word is its abi RTTI vtable address
 * (__class_type_info 0x0209a774); it stays a number because the tree
 * declares no _ZTV array.
 */
#include "types.h"

extern char _ZTSN5dPa_c7level_c10callback_cE[];

/* 0x0208f304: 8 bytes. */
u32 _ZTIN5dPa_c7level_c10callback_cE[2] = {
    0x0209a774,
    (u32)_ZTSN5dPa_c7level_c10callback_cE,
};
