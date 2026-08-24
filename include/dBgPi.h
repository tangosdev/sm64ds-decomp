/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class dBgPi: 5 matched functions, 9 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen.
 *
 * PROMOTED 2026-08-23 to a real polymorphic base for dBgCh_Lin's MI
 * declaration (notes/ctor-migration.md item 2): under C++ the vptr at 0x00
 * is placed implicitly by the first virtual declaration, replacing the old
 * pad_000[0x4]. The ROM's own RTTI names it (_ZTS5dBgPi -> "5dBgPi", vtable
 * data_02099368), its constructor pair is enrolled by real symbol
 * (_ZN5dBgPiC1Ev / _ZN5dBgPiC2Ev -- one out-of-line definition emits both,
 * the ROM kept both copies), and both stay hand-written .c files. The
 * destructor is DECLARED AND NEVER DEFINED AS A METHOD here -- the
 * key-function arrangement from include/ModelBase.h -- so no TU including
 * this header emits a second vtable.
 */
#ifndef DBGPI_H
#define DBGPI_H
#include "types.h"

#ifdef __cplusplus

struct dBgPi {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    s64 unk_004;            /* 0x004 - 4-aligned; the ROM's own offsets prove mwccarm does not 8-align long long here */
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
    u16 unk_018;            /* 0x018 */
    u16 unk_01a;            /* 0x01a */
    s32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    s32 unk_024;            /* 0x024 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~dBgPi();        /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined in this header; the definitions live in the
       hand-written src/_ZN5dBgPiC1Ev.c / _ZN5dBgPiC2Ev.c pair. Declaring it
       is what makes derived constructors emit `bl _ZN5dBgPiC2Ev' instead of
       synthesising the base step inline (notes/ctor-migration.md section 2). */
    dBgPi();

    /* methods */
    void CopyTo(dBgPi &dst) const;
    u32 GetClsnID() const;
};

/* Size is the dBgPi's own span AND the stride of the three consecutive results
   dBgCh_SphCrr holds at 0x74/0x9c/0xc4 -- notes/collision-system.md 3.2. */
typedef char dBgPi_size_must_be_0x28[sizeof(struct dBgPi) == 0x28 ? 1 : -1];

#else

struct dBgPi {
    void **vtable;          /* 0x000 */
    s64 unk_004;            /* 0x004 */
    s32 unk_00c;            /* 0x00c */
    s32 unk_010;            /* 0x010 */
    s32 unk_014;            /* 0x014 */
    u16 unk_018;            /* 0x018 */
    u16 unk_01a;            /* 0x01a */
    s32 unk_01c;            /* 0x01c */
    s32 unk_020;            /* 0x020 */
    s32 unk_024;            /* 0x024 */
};

typedef char dBgPi_size_must_be_0x28[sizeof(struct dBgPi) == 0x28 ? 1 : -1];
typedef struct dBgPi dBgPi;

#endif

#endif
