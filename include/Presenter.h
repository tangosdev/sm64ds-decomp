#ifndef PRESENTER_H
#define PRESENTER_H

#include "types.h"

/* Presenter, the Mario face that reacts in the card and sound games: the
 * 0x1ac-byte record Presenter_Construct builds and the Presenter_* helpers in
 * src/minigames/unit020bfec0.cpp animate. The name is coined. Its model is
 * data/DSMT/face_demo_mario.bmd in front of MG/f_demo_back.bmd, and the eight
 * animation slots resolve (tools/asset_catalog.py) to MG/face_wait,
 * face_mistake, face_win, face_lose, face_lose_st, face_lose_ed, face_smile
 * and face_surprised.bca. A layout view only: the helpers are free functions
 * over a char* record and read it through their PRESENTER() cast. */
struct PresenterFile {
    s32 handle; /* +0x0 */
    s32 file;   /* +0x4 -- the loaded file, 0 until loaded */
};

enum PresenterAnim {
    PRESENTER_ANIM_WAIT,
    PRESENTER_ANIM_MISTAKE,
    PRESENTER_ANIM_WIN,
    PRESENTER_ANIM_LOSE,
    PRESENTER_ANIM_LOSE_START,
    PRESENTER_ANIM_LOSE_END,
    PRESENTER_ANIM_SMILE,
    PRESENTER_ANIM_SURPRISED,
    PRESENTER_ANIM_COUNT
};

struct Presenter {
    s32 mState[2];            /* 0x000 -- the current state's member-pointer record */
    u8 mFace[0x60];           /* 0x008 -- a BlendModelAnim */
    s32 mFaceAnim;            /* 0x068 -- the BCA file mFace plays */
    u8 pad_06c[0xc];          /* 0x06c */
    u8 mBack[0x50];           /* 0x078 -- a Model */
    u8 mTexSeq[0x14];         /* 0x0c8 -- a TextureSequence */
    u8 mTexAnim[0x14];        /* 0x0dc -- a TextureTransformer */
    PresenterFile mFaceModel; /* 0x0f0 -- face_demo_mario.bmd */
    PresenterFile mBackModel; /* 0x0f8 -- f_demo_back.bmd */
    PresenterFile mAnims[PRESENTER_ANIM_COUNT];   /* 0x100 */
    PresenterFile mTexSeqs[PRESENTER_ANIM_COUNT]; /* 0x140 -- one BTP per animation */
    Vector3 mPos;                                 /* 0x180 */
    s32 mSway[4];                                 /* 0x18c -- damped by 0xe00 each frame */
    s32 mTouchX;                                  /* 0x19c -- the stylus point it turns toward */
    s32 mTouchY;                                  /* 0x1a0 */
    s32 mTracking; /* 0x1a4 -- TrackStylus turns toward the touch while set */
    s32 mQuiet;    /* 0x1a8 -- while 0 the Play* calls play their sound (func_02012174) */
    /* Only Presenter_Construct writes 0x1a4 and 0x1a8 (to 0) in the sources read,
       so both names are medium. */
};

#ifndef SM64DS_PLATFORM_PC
typedef char PresenterFile_size_must_be_0x8[sizeof(PresenterFile) == 0x8 ? 1 : -1];
typedef char Presenter_size_must_be_0x1ac[sizeof(Presenter) == 0x1ac ? 1 : -1];
#endif

#endif
