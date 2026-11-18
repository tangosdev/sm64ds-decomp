/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208f304..0x0208f30c, 8 bytes, 1 configured symbols.
 */
#include "types.h"

extern u8 _ZTVN3abi17__class_type_infoE[];
extern char _ZTSN5dPa_c7level_c10callback_cE[];

/* 0x0208f304: 8 bytes. */
u32 _ZTIN5dPa_c7level_c10callback_cE[2] = {
    (u32)_ZTVN3abi17__class_type_infoE,
    (u32)_ZTSN5dPa_c7level_c10callback_cE,
};
