/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Cannon: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CANNON_H
#define CANNON_H
#include "types.h"
#include "Model.h"
#include "dCcAc_c.h"

struct Cannon {
    u8  pad_000[0xd4];
    /* Model member, named by the class's own destructor calling
       Model's D1 at +0x0d4 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6CannonD0Ev.c] */
    Model mModel;            /* 0x0d4 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x124 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN6CannonD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x124 */
    u8  pad_158[0x4];
    /* Where the cannon was placed: InitResources copies mPos here verbatim
       after lowering mPosY by 0x50000. */
    s32 mSpawnPosX;            /* 0x15c */
    s32 mSpawnPosY;            /* 0x160 */
    s32 mSpawnPosZ;            /* 0x164 */
    u8  pad_168[0xc];
    s32 unk_174;            /* 0x174 */
    s16 mAimTimer;            /* 0x178 */
    s16 mTargetAngleY;            /* 0x17a */
    s16 mHomeAngleY;            /* 0x17c */
    u8  pad_17e[0x2];
    /* State index. Behavior calls data_ov098_0213c8fc[mState] as a
       pointer-to-member on `this`. */
    s32 mState;            /* 0x180 */
    /* Cannon variant, the low two bits of the spawn word. */
    u8  unk_184;            /* 0x184 */
    u8  unk_185;            /* 0x185 */
    u8  pad_186[0xe];
    /* Read out of *(*(this + 0xe4) + 0x58) by InitResources -- the last field
       of the object, and what closes the 0x198 Cannon_Spawn allocates. */
    s32 unk_194;            /* 0x194 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    int Render();
#endif
};

typedef char Cannon_size_must_be_0x198[sizeof(struct Cannon) == 0x198 ? 1 : -1];

#endif
