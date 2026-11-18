/* Message font and glyph tables.
 * MSG_GEN_TEXT_FUNCS is the three-entry text-expansion vtable; the 32-byte
 * records are the per-width glyph strips and 0x0208eee0..0x0208f074 the
 * u16 (code, glyph) map Message2/Message3 index.
 * Consumer: src/Message.cpp, Message2.cpp, Message3.cpp.
 * ROM 0x0208ee80..0x0208eecc, 76 bytes, 3 configured symbols.
 *
 * Order: mwcc emits one .data section per object and the linker
 * concatenates them in declaration order, so the definitions below are
 * in the order that yields the ascending addresses above.
 */
#include "types.h"

extern void _Z13func_0201aca4v();
extern void _ZN7Message9AddChoiceEv();
extern void _ZN7Message11AddCharPairEv();

/* 0x0208ee8c: 32 bytes. */
int data_0208ee8c[8] = {
    0x00000000, 0x0000000f, 0x000000ff, 0x00000fff, 0x0000ffff, 0x000fffff, 0x00ffffff, 0x0fffffff,
};

/* 0x0208ee80: 12 bytes. */
void (*MSG_GEN_TEXT_FUNCS[])(void) = {
    (void (*)(void))&_Z13func_0201aca4v,
    (void (*)(void))&_ZN7Message9AddChoiceEv,
    (void (*)(void))&_ZN7Message11AddCharPairEv,
};

/* 0x0208eeac: 32 bytes. */
int data_0208eeac[8] = {
    (int)0xffffffffu, (int)0xfffffff0u, (int)0xffffff00u, (int)0xfffff000u,
    (int)0xffff0000u, (int)0xfff00000u, (int)0xff000000u, (int)0xf0000000u,
};
