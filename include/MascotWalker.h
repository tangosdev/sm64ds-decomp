#ifndef MASCOTWALKER_H
#define MASCOTWALKER_H

#include "types.h"

/* MascotWalker, the small figure that walks the card table's paths in the card
 * games (the dealer embeds one at +0xdc, which fixes the 0xf8 size). The name
 * is coined. A layout view only: the MascotWalker_* helpers in
 * src/minigames/unit020bfec0.cpp are free functions over a char* record and
 * read it through their WALKER() cast. Unnamed regions are touched in one
 * place at most, or sit inside an embedded object. */
struct MascotWalker {
    u8 pad_00[0x14];  /* 0x00 */
    s32 mWalkAnim;    /* 0x14 -- the BCA file StartNextPath restarts */
    u8 mModel[0x1c];  /* 0x18 -- a BlendModelAnim */
    u8 mMatrix[0x30]; /* 0x34 -- a Matrix4x3, set by UpdateMatrix */
    u8 pad_64[0x4c];  /* 0x64 */
    s32 mPath;        /* 0xb0 -- the path walked: a point count, then the points */
    s32 mState[2];    /* 0xb4 -- the current state's member-pointer record */
    u8 pad_bc[0xc];   /* 0xbc */
    Vector3 mPos;     /* 0xc8 */
    Vector3 mDir;     /* 0xd4 -- unit vector toward the next point */
    s32 mPathPoint;   /* 0xe0 -- the point being walked to */
    s32 mPathIndex;   /* 0xe4 -- which of the four paths, cycling */
    u8 pad_e8[0x2];   /* 0xe8 */
    s16 mAngleY;      /* 0xea -- eased toward mTargetAngle */
    u8 pad_ec[0x2];   /* 0xec */
    s16 mTargetAngle; /* 0xee -- horizontal angle to the next point */
    u8 pad_f0[0x8];   /* 0xf0 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char MascotWalker_size_must_be_0xf8[sizeof(MascotWalker) == 0xf8 ? 1 : -1];
#endif

#endif
