//cpp
// @symbol _ZN5Event6GetBitEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
#include "types.h"
/* Event::GetBit(u32 bit) at 0x02029ee0
 * Free function in namespace Event (no `this`). Returns nonzero if bit `bit`
 * is set in the global event bitfield data_0209f34c. Declared in SM64DS_2.h:
 *   namespace Event { u32 GetBit(u32 bit); ... }
 *
 * Reloc resolves to data_0209f34c at 0x0209f34c (symbols.txt: data_0209f34c).
 * The `1 << n` shift is load-bearing.
 */
extern "C" {
extern s32 data_0209f34c; /* 0x0209f34c */
}

namespace Event {

s32 GetBit(u32 bit)
{
    return data_0209f34c & (1 << bit);
}

}
