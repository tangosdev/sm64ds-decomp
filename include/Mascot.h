#ifndef MASCOT_H
#define MASCOT_H

#include "types.h"

/* Mascot, Toad standing at the table in the slot and card games: the 0xac-byte
 * record Mascot_Construct builds and the Mascot_* helpers in
 * src/minigames/unit020bfec0.cpp draw. The name is coined. Its files resolve
 * (tools/asset_catalog.py) to MG/kino_d.bmd and MG/kino_wait.bca. A layout
 * view only: the helpers are free functions over a char* record and read it
 * through their MASCOT() cast. */
struct Mascot {
    s32 mModelFile[2];    /* 0x00 -- kino_d.bmd: handle, then the loaded file */
    s32 mWaitAnimFile[2]; /* 0x08 -- kino_wait.bca: handle, then the loaded file */
    u8 mModel[0x70];      /* 0x10 -- a BlendModelAnim */
    Vector3 mLookTarget;  /* 0x80 -- what Mascot_LookAt turns the head toward */
    Vector3 mPos;         /* 0x8c */
    s16 mRot[3];          /* 0x98 -- [1] is the body's yaw, eased toward the target */
    u8 pad_9e[0x4];       /* 0x9e */
    s16 mHeadPitch;       /* 0xa2 -- clamped to +/-0x1000 */
    s16 mHeadYaw;         /* 0xa4 -- clamped to +/-0x2800 of the body */
    u8 pad_a6[0x2];       /* 0xa6 */
    s32 mLooking;         /* 0xa8 -- set by Mascot_LookAt, cleared by Mascot_StopLooking */
};

#ifndef SM64DS_PLATFORM_PC
typedef char Mascot_size_must_be_0xac[sizeof(Mascot) == 0xac ? 1 : -1];
#endif

#endif
