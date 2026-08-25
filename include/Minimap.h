#ifndef MINIMAP_H
#define MINIMAP_H

#include "types.h"
#include "dBase_c.h"

/* Derives from dBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x50 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#ifdef __cplusplus

struct Minimap : dBase_c {
    /* The BG's 2x2 affine matrix, in the order the hardware wants it:
       Behavior fills A and B from the sin/cos table data_02082214 scaled by
       mInvScale, then sets C = -B and D = A -- a rotate-and-scale -- and hands
       &mBgMatrixA to UpdateMinimap as the four-word block. Seeded 1,0,0,1 in
       Fix12 by InitResources. D was called mPosX, which the `mBgMatrixD =
       mBgMatrixA` in Behavior makes untenable. */
    s32 mBgMatrixA;                      /* 0x050 */
    s32 mBgMatrixB;                      /* 0x054 */
    s32 mBgMatrixC;                      /* 0x058 */
    s32 mBgMatrixD;                        /* 0x05c */
    s32 mMapCenterX;                      /* 0x060 */
    s32 mMapCenterY;                      /* 0x064 */
    u8  pad_068[0x8];
    s32 mPlayerIconX[4];                   /* 0x070 */
    s32 mPlayerIconY[4];                   /* 0x080 */
    s32 unk_090;                      /* 0x090 */
    s32 unk_094;                      /* 0x094 */
    s32 unk_098;                      /* 0x098 */
    s32 unk_09c;                      /* 0x09c */
    s32 mStarIconX[12];                  /* 0x0a0 */
    s32 mStarIconY[12];                  /* 0x0d0 */
    s32 mCapIconX[9];                   /* 0x100 */
    s32 mCapIconY[9];                   /* 0x124 */
    u8  pad_148[0x30];
    s32 mStarKeyIconX;                      /* 0x178 */
    s32 mStarKeyIconY;                      /* 0x17c */
    s32 mSpikeBombIconX[8];                   /* 0x180 */
    s32 mSpikeBombIconY[8];                   /* 0x1a0 */
    u8  pad_1c0[0x14];
    s32 mScale;                      /* 0x1d4 */
    s32 mMapWidth;                      /* 0x1d8 */
    s32 mMapCenterOffset;                      /* 0x1dc */
    /* Where the level wants its map centred, in world coordinates. A per-level
       constant: InitResources picks 0x258000/0/0x64000 for one level group,
       -0x2bc000/0/-0x2bc000 for another and zero otherwise, and nothing else
       writes it. Behavior copies it into mMapCenterWorld* each frame. */
    s32 mMapOriginX;                      /* 0x1e0 */
    s32 mMapOriginY;                      /* 0x1e4 */
    s32 mMapOriginZ;                      /* 0x1e8 */
    s32 unk_1ec;                      /* 0x1ec */
    s32 mCurrentScale;                      /* 0x1f0 */
    /* The map centre actually used this frame, in world coordinates. Behavior
       seeds it from mMapOrigin*, converts the player's position through
       Minimap::GetPosOnMinimap, clamps the icon inside the visible window,
       converts back with Minimap::GetPosFromMinimapPos and adds the difference
       here -- so the map scrolls only as far as it must to keep the player on
       screen. Every icon on the map is projected relative to it. */
    s32 mMapCenterWorldX;                      /* 0x1f4 */
    s32 mMapCenterWorldY;                      /* 0x1f8 */
    s32 mMapCenterWorldZ;                      /* 0x1fc */
    /* The direction arrow's 2x2 OAM matrix, same four-word shape as
       mBgMatrix*: Render fills A and B from data_02082214 scaled by
       mArrowScale and sets C = -B, D = A, in both the VS and single-player
       branches, immediately before drawing OAM::MM_ARROW. */
    s32 mArrowMatrixA;                      /* 0x200 */
    s32 mArrowMatrixB;                      /* 0x204 */
    s32 mArrowMatrixC;                      /* 0x208 */
    s32 mArrowMatrixD;                      /* 0x20c */
    s32 mArrowScale;                      /* 0x210 */
    s32 mTargetInvScale;                      /* 0x214 */
    s32 mInvScale;                      /* 0x218 */
    u16 mAngle;                      /* 0x21c */
    s8 mPlayerMapIDs[4];                    /* 0x21e */
    s8 mStarMapIDs[12];                   /* 0x222 */
    /* Per-star animation cursor. Render advances entry i by data_0208ee44,
       wraps it at 12, and uses `% 12 > 4` to pick between the two frames of a
       star marker; it is reset to 0 for any star not on the current map. Was
       flat padding. */
    u8 mStarIconAnimFrame[12];             /* 0x22e */
    s8 mCapMapIDs[9];                    /* 0x23a */
    u8  pad_243[0x5];
    s8 mStarKeyMapID;                       /* 0x248 */
    s8 mSpikeBombMapIDs[8];                    /* 0x249 */
    u8 mArrowType;                       /* 0x251 */
    u8  pad_252[0x2];
    u8 mTouchCircleTimer;                       /* 0x254 */
    u8 mInIntroCutscene;                       /* 0x255 */
    /* Render increments it, wraps it at 5 and draws the star key's first
       frame while it is under 2 -- a blink. The header did not reach this far;
       Render's own shadow of the class did. */
    u8 mStarKeyBlinkTimer;                     /* 0x256 */

    /* --- vtable --- */
    virtual ~Minimap();

    int Behavior();
    static void FixTHIPaintingRoomPos(Vector3 & v_);
    static void UpdateLevelSpecific();
    int InitResources();
    int Render();
    s32 CleanupResources();
    void OnPendingDestroy();
};

typedef char Minimap_size_must_be_0x258[sizeof(Minimap) == 0x258 ? 1 : -1];

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct Minimap {
    u8  pad_000[0x50];
    /* Rotate-and-scale 2x2 for the minimap BG; see the C++ spelling above. */
    s32 mBgMatrixA;            /* 0x050 */
    s32 mBgMatrixB;            /* 0x054 */
    s32 mBgMatrixC;            /* 0x058 */
    s32 mBgMatrixD;            /* 0x05c */
    s32 mMapCenterX;            /* 0x060 */
    s32 mMapCenterY;            /* 0x064 */
    u8  pad_068[0x8];
    /* Minimap::Behavior carries a shadow struct that declares these ranges as
       ARRAYS and indexes them; the header had them as flat padding, which is
       why the shadow existed at all. Sizes are the shadow's own, and the
       reconstruction is offset-neutral -- the struct still spans 0x256. */
    s32 mPlayerIconX[4];            /* 0x070 */
    s32 mPlayerIconY[4];            /* 0x080 */
    s32 unk_090;            /* 0x090 */
    s32 unk_094;            /* 0x094 */
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 mStarIconX[12];            /* 0x0a0 */
    s32 mStarIconY[12];            /* 0x0d0 */
    s32 mCapIconX[9];            /* 0x100 */
    s32 mCapIconY[9];            /* 0x124 */
    u8  pad_148[0x30];
    s32 mStarKeyIconX;            /* 0x178 */
    s32 mStarKeyIconY;            /* 0x17c */
    s32 mSpikeBombIconX[8];            /* 0x180 */
    s32 mSpikeBombIconY[8];            /* 0x1a0 */
    u8  pad_1c0[0x14];
    s32 mScale;            /* 0x1d4 */
    s32 mMapWidth;            /* 0x1d8 */
    s32 mMapCenterOffset;            /* 0x1dc */
    /* 0x1e0 and 0x1f4 are each a Vector3 in the shadow -- three consecutive
       words handed to Vec3 helpers as one object. Kept as scalars here so the
       header does not assert a type the other eight matched functions never
       see, and taken as `*(Vector3*)&mMapOriginX` at the two sites that need it. */
    s32 mMapOriginX;            /* 0x1e0 */
    s32 mMapOriginY;            /* 0x1e4 */
    s32 mMapOriginZ;            /* 0x1e8 */
    s32 unk_1ec;            /* 0x1ec */
    s32 mCurrentScale;            /* 0x1f0 */
    s32 mMapCenterWorldX;            /* 0x1f4 */
    s32 mMapCenterWorldY;            /* 0x1f8 */
    s32 mMapCenterWorldZ;            /* 0x1fc */
    s32 mArrowMatrixA;            /* 0x200 */
    s32 mArrowMatrixB;            /* 0x204 */
    s32 mArrowMatrixC;            /* 0x208 */
    s32 mArrowMatrixD;            /* 0x20c */
    s32 mArrowScale;            /* 0x210 */
    s32 mTargetInvScale;            /* 0x214 */
    s32 mInvScale;            /* 0x218 */
    /* Stays u16. Behavior's shadow spelled it s16 and then cast EVERY read to
       (u16) -- `(u16)ang >= 0x8000` and `(s32)(u16)ang >> 4` -- so the reads
       want unsigned and the two spellings mean the same thing. */
    u16 mAngle;            /* 0x21c */
    s8  mPlayerMapIDs[4];            /* 0x21e */
    s8  mStarMapIDs[12];            /* 0x222 */
    /* Per-star animation cursor. Render advances entry i by data_0208ee44,
       wraps it at 12, and uses `% 12 > 4` to pick between the two frames of a
       star marker; it is reset to 0 for any star not on the current map. Was
       flat padding. */
    u8 mStarIconAnimFrame[12];             /* 0x22e */
    s8  mCapMapIDs[9];            /* 0x23a */
    u8  pad_243[0x5];
    s8  mStarKeyMapID;            /* 0x248 */
    s8  mSpikeBombMapIDs[8];            /* 0x249 */
    u8  mArrowType;            /* 0x251 */
    u8  pad_252[0x2];
    u8  mTouchCircleTimer;            /* 0x254 */
    u8  mInIntroCutscene;            /* 0x255 */
    u8  mStarKeyBlinkTimer;          /* 0x256 */
};

#endif /* __cplusplus */

#endif /* MINIMAP_H */
