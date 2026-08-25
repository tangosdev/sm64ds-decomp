/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class MegaMushroomCreateTag: 4 matched functions, 3 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MEGAMUSHROOMCREATETAG_H
#define MEGAMUSHROOMCREATETAG_H
#include "types.h"
#include "dCcAc_c.h"

struct MegaMushroomCreateTag {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u8  pad_00c[0xc8];
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN21MegaMushroomCreateTagD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x0d4 */
    /* Five bytes past the cylinder, and Behavior reads or writes every one of
       them through its own flat view of the object -- which is what closes the
       0x110 MegaMushroomCreateTag_Spawn allocates. */
    u8  unk_108;            /* 0x108 */
    s8  unk_109;            /* 0x109 */
    u8  unk_10a;            /* 0x10a */
    u8  unk_10b;            /* 0x10b */
    u8  unk_10c;            /* 0x10c */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
#endif
};

typedef char MegaMushroomCreateTag_size_must_be_0x110[
    sizeof(struct MegaMushroomCreateTag) == 0x110 ? 1 : -1];

#endif
