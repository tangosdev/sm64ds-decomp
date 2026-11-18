/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208f278..0x0208f2d0, 88 bytes, 2 configured symbols.
 */
#include "types.h"

/* 0x0208f278: 36 bytes. */
u16 data_0208f278[18] = {
    'S', 'u', 'p', 'e', 'r', ' ', 'M', 'a', 'r', 'i', 'o', ' ', '6', '4', ' ', 'D', 'S', 0x0000,
};

/* 0x0208f29c: 52 bytes. */
char data_0208f29c[52] = "/data/2D_cad/d_2d_MARIO64DS_IPL_download_ncg.bin";
