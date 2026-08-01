/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class ActorBase: 21 matched functions, 8 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef ACTORBASE_H
#define ACTORBASE_H
#include "types.h"

/* fwd */
struct a;
struct vfSuccess_;
struct ActorBase {
    u8  pad_000[0xe];
    u8  unk_00e;            /* 0x00e */
    u8  mMarkedForDestruction;            /* 0x00f */
    u8  unk_010;            /* 0x010 */
    u8  pad_011[0x2];
    u8  unk_013;            /* 0x013 */
    u8  pad_014[0x4];
    s32 unk_018;            /* 0x018 */
    u8  pad_01c[0xc];
    u8  unk_028;            /* 0x028 */
    u8  pad_029[0xf];
    u8  unk_038;            /* 0x038 */
    u8  pad_039[0xf];
    s32 unk_048;            /* 0x048 */
#ifdef __cplusplus
    /* methods */
    bool BeforeInitResources();
    bool OnHeapCreated();
    int BeforeBehavior();
    int BeforeCleanupResources();
    int BeforeRender();
    s32 Behavior();
    s32 CleanupResources();
    s32 InitResources();
    s32 Render();
    void AfterBehavior(unsigned int vfSuccess_);
    void AfterInitResources(unsigned int a);
    void AfterRender(unsigned int vfSuccess_);
    void MarkForDestruction();
    void OnPendingDestroy();
#endif
};

#endif
