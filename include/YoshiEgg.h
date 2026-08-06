/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class YoshiEgg: 6 matched functions, 12 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef YOSHIEGG_H
#define YOSHIEGG_H
#include "types.h"

/* Player is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef PLAYER_FWD_DECLARED
#define PLAYER_FWD_DECLARED
struct Player;
typedef struct Player Player;
#endif

struct YoshiEgg {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x48];
    u32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x5c];
    u8  mMovingCylinderClsn;            /* 0x110 */
    u8  pad_111[0x33];
    u8  mWithMeshClsn;            /* 0x144 */
    u8  pad_145[0x1bb];
    u8  mModelAnim;            /* 0x300 */
    u8  pad_301[0x4f];
    u8  mAnimation;            /* 0x350 */
    u8  pad_351[0x13];
    u8  mShadowModel;            /* 0x364 */
    u8  pad_365[0x27];
    /* Player * -- the ROM loads this WORD and passes it to _ZN6Player16IsInsideOfCannonEv
       as that function's `this`, which is an object address, so the word is a Player *. It
       says nothing about the rest of the marker's span, which stays explicit padding. Was
       a u8 marker. */
    Player *mPlayer;            /* 0x38c */
    u8  pad_390[0x60];
    s32 unk_3f0;            /* 0x3f0 */
    u8  pad_3f4[0x2c];
    u8  unk_420;            /* 0x420 */
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int CleanupResources();
    int Render();
#endif
};

#endif
