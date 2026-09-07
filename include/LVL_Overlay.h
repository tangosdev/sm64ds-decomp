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
 *   ObjSubTable  twelve of the fifteen sub-table loaders read their count as
 *                `*(u8 *)(t + 1)' -- an 8-bit load at offset 0x01. The other
 *                three read no count at all: LoadStarCameraObjects and
 *                LoadUnusedType13Objects each take a single value out of the
 *                entry array, and LoadPathNodeObjects forwards the array
 *                pointer without a length. None of the fifteen reads a u16 at
 *                offset 0x00.
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
 * entries whose byte 0 is packed two ways -- so the pointer is untyped here and
 * each loader says what it is pointing at. Naming those record types is
 * per-category work, not something to guess once for all of them.
 *
 * On that packing, because an earlier revision of this comment had it backwards:
 * the object TYPE is the LOW FIVE bits (`b & 0x1f'), used to index the handler
 * table at ov002:0x0210cbb8. The TOP THREE bits (`(b >> 5) & 7') are a filter --
 * LoadObjects skips the entry unless they are zero or equal to the global byte
 * at 0x0209f220. Type in the low bits, filter in the high bits, not the other
 * way round.
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

    /* ==== PER-CATEGORY RECORD TYPES ====
       `entries' is void* because these differ, and each is added here as its
       loader is migrated rather than guessed in advance. Positions are stored as
       s16 world units and shifted left by 12 into Fix12 on the way out, which is
       why every one of them is a short and every loader does `<< 12'. */

    /* 0xe bytes. LoadExitObjects (ov002:0x020fe420). */
    struct ExitEntry {
        s16 x;                  /* 0x00 */
        s16 y;                  /* 0x02 */
        s16 z;                  /* 0x04 */
        s16 rotX;               /* 0x06 -- NEGATED on use, both of them */
        s16 rotY;               /* 0x08 */
        u8  param[4];           /* 0x0a -- assembled BIG-ENDIAN: byte 0x0a is the
                                   most significant. The rest of this file is
                                   little-endian, so this is a stored big-endian
                                   word and not an accident of the recovery --
                                   the ROM really does shift 0x0a by 24. */
    };

    /* 8 bytes. LoadTeleportSourceObjects (ov002:0x020fe5f4). No rotation: the
       spawn call passes NULL for it. */
    struct TeleportSourceEntry {
        s16 x;                  /* 0x00 */
        s16 y;                  /* 0x02 */
        s16 z;                  /* 0x04 */
        u16 param;              /* 0x06 */
    };

    /* 0x10 bytes. LoadStandardObjects (ov002:0x020fe8ac) and
       LoadEntranceObjects (ov002:0x020fe6c8). `raw' indexes
       data_ov002_0210cbf4 for the actor id; positions are s16 world units
       shifted left by 12 into Fix12 on spawn, same as every other loader. */
    struct StandardEntry {
        u16      raw;           /* 0x00 -- object id index */
        s16      x;             /* 0x02 */
        s16      y;             /* 0x04 */
        s16      z;             /* 0x06 */
        Vector3s rot;           /* 0x08 */
        u16      param;         /* 0x0e */
    };

    /* 8 bytes. LoadSimpleObjects (ov002:0x020fe960). No rotation field: spawn
       passes NULL for it. `raw' packs a 9-bit actor index and a parameter in
       the high bits; 0x1ff is the special minimap-change sentinel. */
    struct SimpleEntry {
        u16 raw;                /* 0x00 -- low 9 bits: actor index; high: param */
        s16 x;                  /* 0x02 */
        s16 y;                  /* 0x04 */
        s16 z;                  /* 0x06 */
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
typedef char LVL_Overlay_ExitEntry_size_must_be_0xe[
    sizeof(LVL_Overlay::ExitEntry) == 0xe ? 1 : -1];
typedef char LVL_Overlay_TeleportSourceEntry_size_must_be_0x8[
    sizeof(LVL_Overlay::TeleportSourceEntry) == 0x8 ? 1 : -1];
typedef char LVL_Overlay_StandardEntry_size_must_be_0x10[
    sizeof(LVL_Overlay::StandardEntry) == 0x10 ? 1 : -1];
typedef char LVL_Overlay_SimpleEntry_size_must_be_0x8[
    sizeof(LVL_Overlay::SimpleEntry) == 0x8 ? 1 : -1];
#endif

#endif
