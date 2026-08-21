#ifndef PATHLIFT_H
#define PATHLIFT_H

#include "types.h"

/* A lift that runs along a path. Derives from dBgActor_c, and its own data starts
 * in the base's tail padding at 0x31e (dBgActor_c's last field ends there and its
 * size rounds to 0x320).
 *
 * THE ARRAY IS Model[3]. The ROM destroys it with
 * __destroy_arr(this + 0x320, 3, 0x50, _ZN5ModelD1Ev) -- the element
 * destructor is named in the ROM already, and 0x50 is sizeof(Model), so this
 * one needed no new type at all. 0x320 + 3 * 0x50 = 0x410.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "Model.h"

struct PathLift : dBgActor_c {
    u8  pad_31e[0x2];
    Model mModels[3];                     /* 0x320 */
    u8  pad_410[0x1a];
    u8  unk_42a;                          /* 0x42a */
    u8  unk_42b;                          /* 0x42b */
    u8  pad_42c[0x20];
    s32 mState;                          /* 0x44c */

    virtual ~PathLift();

    void AfterClsn();
    /* Non-virtual. Subclasses (e.g. daObjPathLift_c) call it from their own
       Behavior override to run PathLift's shared step first. */
    void BaseBehavior();
};

typedef char PathLift_size_must_be_0x450[sizeof(PathLift) == 0x450 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct PathLift {
    u8  pad_000[0xc];
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0xc6];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMovingMeshCollider;            /* 0x124 */
    u8  pad_125[0x305];
    u8  unk_42a;            /* 0x42a */
    u8  unk_42b;            /* 0x42b */
    u8  pad_42c[0x20];
    s32 mState;            /* 0x44c */
};

#endif /* __cplusplus */

#endif /* PATHLIFT_H */
