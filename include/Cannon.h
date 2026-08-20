/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Cannon: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CANNON_H
#define CANNON_H
#include "types.h"
#include "Model.h"

struct Cannon {
    u8  pad_000[0xd4];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6CannonD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    u8  mdCcAc_c;            /* 0x124 */
    u8  pad_125[0x5b];
    s32 unk_180;            /* 0x180 */
    u8  pad_184[0x1];
    u8  unk_185;            /* 0x185 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int InitResources();
    int Render();
#endif
};

#endif
