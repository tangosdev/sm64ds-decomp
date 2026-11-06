#ifndef MGBANNER_H
#define MGBANNER_H

#include "types.h"

/* One of the three 0x134-byte records at gMgBanners: a graphic
   loaded from the file table and drawn as a list of OAM records. The class
   name MgBanner is the one its constructor and destructor symbols carry; the
   real class is not recovered. Its handlers and constructor are defined in
   src/minigames/unit020b2c84.cpp; dScMgBase_c's unit draws, updates and frees
   the three records. */
struct MgBanner;
typedef void (MgBanner::*ElemFn)();
typedef char ElemFn_size_must_be_0x8[sizeof(ElemFn) == 0x8 ? 1 : -1];
struct MgBanner {
    ElemFn mBehavior; /* 0x00 run by Banner_Update */
    ElemFn mRender;   /* 0x08 run by Banner_Draw; 0 draws plainly */
    s16 mX, mY;       /* 0x10 screen position, pixels */
    s16 mTargetX;     /* 0x14 where the slide-in handlers stop */
    s16 mTargetY;     /* 0x16 */
    int mPriority;    /* 0x18 passed right after mScreen to every draw call */
    int mScreen;      /* 0x1c -1 draws on both screens (Mg_RenderSpriteOn) */
    int mFileIndex;   /* 0x20 index into the file table; 29 when free */
    int mTimer;       /* 0x24 mostly stepped 0x1000 (1.0) per frame toward
                            mTimerEnd; Banner_UpdateDrop counts it by 1 */
    int mTimerEnd;    /* 0x28 */
    s16 mOamCount;    /* 0x2c records Mg_CopySpriteList copied */
    s16 mStyle;       /* 0x2e index into the three handler tables */
    s16 mGfxSlot;     /* 0x30 0..2, see sMgBannerGfxSlotUsed */
    u16 mArrived;     /* 0x32 set when mX reaches mTargetX
                            (Banner_UpdateSlideFromRight, Banner_UpdateSlideFromLeft) */
    u8 mOam[0x100];   /* 0x34 GXOamAttr records; the one whose attr3 is 0xffff is the
                            last drawn. The squeeze and zoom draws read x, the 9-bit
                            screen position, and fold values above 0x100 negative. */
    MgBanner();
    ~MgBanner() {}
};
typedef char MgBanner_size_must_be_0x134[sizeof(MgBanner) == 0x134 ? 1 : -1];

#endif
