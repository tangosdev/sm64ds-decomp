/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Skeeter: 5 matched functions, 22 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef SKEETER_H
#define SKEETER_H
#include "types.h"
#include "ModelAnim.h"
#include "MovingCylinderClsnWithPos.h"
#include "WithMeshClsn.h"

struct Skeeter {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x18];
    s32 mScaleX;            /* 0x080 */
    s32 mScaleY;            /* 0x084 */
    s32 mScaleZ;            /* 0x088 */
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    s32 unk_0ac;            /* 0x0ac */
    u32 mFlags;            /* 0x0b0 */
    u8  pad_0b4[0x18];
    s8  unk_0cc;            /* 0x0cc */
    u8  pad_0cd[0x33];
    u16 unk_100;            /* 0x100 */
    u8  pad_102[0x2];
    u16 unk_104;            /* 0x104 */
    u8  pad_106[0x1];
    u8  unk_107;            /* 0x107 */
    u8  unk_108;            /* 0x108 */
    u8  pad_109[0x1];
    u8  unk_10a;            /* 0x10a */
    u8  pad_10b[0x1];
    /* Death/hit state. Zero is alive; non-zero routes Behavior into the death
       branch, and 1 specifically is the state func_020aea30 installs when the
       skeeter is knocked out over water. */
    s32 unk_10c;            /* 0x10c */
    /* MovingCylinderClsnWithPos member, named by the class's own destructor calling
       MovingCylinderClsnWithPos's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkeeterD1Ev.c] */
    MovingCylinderClsnWithPos mMovingCylinderClsnWithPos;            /* 0x110 */
    /* WithMeshClsn member, named by the class's own destructor calling
       WithMeshClsn's D1 at +0x150 -- a relocation the ROM build
       checks. Was a u8 marker. [_ZN7SkeeterD1Ev.c] */
    WithMeshClsn mWithMeshClsn;            /* 0x150 */
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0x30c -- a relocation the ROM build
       checks. D1 and not D2, so it is this type and not an inlined base. The marker's pad
       ran 0x4 bytes PAST the end of the object; that space is not evidenced and stays
       explicit padding rather than being folded into the member. */
    ModelAnim mModelAnim;            /* 0x30c */
    /* Current state: a record whose third word is a pointer-to-member tick,
       null-checked before the call. func_ov090_02131e00 installs one. */
    void* mState;            /* 0x370 */
    s32 unk_374;            /* 0x374 */
    s32 unk_378;            /* 0x378 */
    s32 unk_37c;            /* 0x37c */
    u8  pad_380[0x14];
    u16 unk_394;            /* 0x394 */
    u16 unk_396;            /* 0x396 */
    u16 unk_398;            /* 0x398 */
    u16 unk_39a;            /* 0x39a */
    u8  unk_39c;            /* 0x39c */
    u8  pad_39d[0x4];
    u8  unk_3a1;            /* 0x3a1 */
    u8  pad_3a2[0x2];
    s32 unk_3a4;            /* 0x3a4 */
    s32 unk_3a8;            /* 0x3a8 */
    s32 unk_3ac;            /* 0x3ac */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
#endif
};

#endif
