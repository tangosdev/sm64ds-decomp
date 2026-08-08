/* Hand-written, against evidence. There has never been a header for this type:
 * every one of the eighteen level-loading functions that takes one declared its
 * own copy, and they do not agree with each other.
 *
 * LVL_Overlay is the parsed form of a level's object data. It holds a table per
 * object category, and a loader function per category walks it.
 *
 * ==== TWO DIFFERENT TABLE TYPES, AND THE EVIDENCE THAT THEY DIFFER ====
 *
 * The mangled names distinguish them -- `RN11LVL_Overlay8ObjTableE' versus
 * `RN11LVL_Overlay11ObjSubTableE' -- and so do the bodies:
 *
 *   ObjTable     LoadObjects (ov002:0x020fe33c) reads its count as
 *                `*(unsigned short *)t' -- a 16-bit load at offset 0x00.
 *
 *   ObjSubTable  every sub-table loader reads its count as `*(u8 *)(t + 1)' --
 *                an 8-bit load at offset 0x01.
 *
 * Those cannot be the same field. On little-endian, byte 1 of a u16 at 0x00 is
 * the HIGH byte, so a u8 read at 0x01 of a 16-bit count would report zero for
 * every count below 256 -- which is every count. So ObjTable really does carry a
 * u16 at 0x00, ObjSubTable really does carry a u8 at 0x01, and offset 0x00 of an
 * ObjSubTable is something else that no loader in this family reads.
 *
 * Both are 8 bytes with the entry pointer at 0x04: every loader reaches it as
 * `*(char **)(t + 4)'.
 *
 * The element type of `entries' is deliberately void*. It differs per category
 * -- LoadDoorObjects walks 12-byte door records, LoadObjects walks 8-byte
 * entries with a packed type in the top three bits of byte 0 -- so the pointer
 * is untyped here and each loader says what it is pointing at. Naming those
 * record types is per-category work, not something to guess once for all of
 * them.
 */
#ifndef LVL_OVERLAY_H
#define LVL_OVERLAY_H
#include "types.h"

struct LVL_Overlay {
#ifdef __cplusplus
    /* Nested so that the compiler mangles `N11LVL_Overlay8ObjTableE' and
       `N11LVL_Overlay11ObjSubTableE' -- which is what the ROM's symbols say,
       and the reason these cannot simply be two top-level structs. */
    struct ObjTable {
        u16   count;            /* 0x00 -- 16-bit; see the banner */
        u8    pad_002[2];
        void* entries;          /* 0x04 */
    };

    struct ObjSubTable {
        u8    unk_000;          /* 0x00 -- not read by any loader in this family */
        u8    count;            /* 0x01 -- 8-bit; see the banner */
        u8    pad_002[2];
        void* entries;          /* 0x04 */
    };
#endif
};

#ifdef __cplusplus
/* tools/check_header_offsets.py CANNOT GATE THIS HEADER. Its aggregate regex is
   `struct X { [^{}]* }', which by construction cannot span a nested brace, so it
   walks the outer LVL_Overlay, finds no fields of its own and reports
   "0 commented fields, 0 mismatched, struct spans 0x0" -- which reads like a
   pass and checks nothing. The same shape of silent no-op that putting the C
   spelling first fixed for include/Heap.h; here there is no reordering that
   helps, because the types being described ARE the nested ones.

   So the sizes are asserted at compile time instead. This is narrower than the
   offset gate -- it catches a wrong width or a missing pad, not a wrong order --
   but it is a real check that fails the build rather than a report that passes
   without looking. The tree already uses this idiom, and
   check_header_offsets.py reads these names into its CLASS_SIZES table. */
typedef char LVL_Overlay_ObjTable_size_must_be_0x8[
    sizeof(LVL_Overlay::ObjTable) == 0x8 ? 1 : -1];
typedef char LVL_Overlay_ObjSubTable_size_must_be_0x8[
    sizeof(LVL_Overlay::ObjSubTable) == 0x8 ? 1 : -1];
#endif

#endif
