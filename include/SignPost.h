/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base by hand.
 *
 * class SignPost: 5 matched functions, 24 evidenced fields. Offsets and widths
 * below 0x320 are gone from this header and inherited now; the rest are still
 * observed, with gaps as explicit padding. Field NAMES are placeholders.
 *
 * THE BASE IS dBgActor_c, and the destructor says so outright. _ZN8SignPostD1Ev
 * (ov002 0x020badd0) stores exactly two vtables -- _ZTV15daObjTatefuda_c, then
 * _ZTV10dBgActor_c -- with this class's own three members destroyed between them
 * and dBgActor_c's Model and MovingMeshCollider destroyed after. Two stores is a
 * DIRECT child; an intermediate layer would have put a third one in between.
 * SignPost_Spawn agrees from the other side: it calls _ZN10dBgActor_cC2Ev and then
 * overwrites the vptr with _ZTV8SignPost, which is the same address as
 * _ZTV15daObjTatefuda_c -- one table, two names.
 *
 * SIZE IS 0x5a4, the literal SignPost_Spawn passes to fBase_c::operator new
 * (1444). The last named field, the Player * at 0x59c, closes at 0x5a0, so four
 * bytes of tail stay padding.
 *
 * THE THREE MEMBERS ARE PLACED BY THE FACTORY, not by field-span guesswork:
 * SignPost_Spawn runs MovingCylinderClsn's constructor at this+0x320,
 * ShadowModel's at this+0x358 and WithMeshClsn's at this+0x3c8, and the
 * destructor tears the same three down in reverse. Two of them are typed here
 * and each closes on its own class's size assert -- MovingCylinderClsn 0x34 ends
 * at 0x354, ShadowModel 0x28 ends at 0x380 -- which is a second check on both
 * offsets. Typing them is not cosmetic: `(char *)&member` on a u8 marker in the
 * DERIVED part of this object made mwcc materialise the offset through the
 * literal pool (ldr + add where the ROM has a single add), and InitResources
 * came out 0x10 long until those two were given their real types and called as
 * methods. The third, mWithMeshClsn, is still a u8 marker; its call site never
 * had the problem, so it was left alone rather than widened opportunistically.
 *
 * 0x320 IS ALSO WHY sizeof(dBgActor_c) IS 0x320 -- include/dBgActor_c.h names this
 * class (as WallSign's sibling daObjKanban_c does) among the four direct
 * children that each place a 4-byte-aligned class member there.
 */
#ifndef SIGNPOST_H
#define SIGNPOST_H
#include "types.h"

/* Player is only ever pointed at from here, so a declaration is enough --
 * no definition is pulled in. The typedef keeps the member spelled the
 * same in C and in C++; the guard is common.h's idiom for the same job. */
#ifndef PLAYER_FWD_DECLARED
#define PLAYER_FWD_DECLARED
struct Player;
typedef struct Player Player;
#endif

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "MovingCylinderClsn.h"
#include "ShadowModel.h"

struct SignPost : dBgActor_c {
    MovingCylinderClsn mMovingCylinderClsn;  /* 0x320 */
    u8  pad_354[0x4];
    ShadowModel mShadowModel;   /* 0x358 */
    u8  pad_380[0x30];
    s32 unk_3b0;                /* 0x3b0 */
    s32 unk_3b4;                /* 0x3b4 */
    s32 unk_3b8;                /* 0x3b8 */
    s16 unk_3bc;                /* 0x3bc */
    s16 unk_3be;                /* 0x3be */
    s16 unk_3c0;                /* 0x3c0 */
    u8  pad_3c2[0x6];
    u8  mWithMeshClsn;          /* 0x3c8 */
    u8  pad_3c9[0x1c5];
    u8  unk_58e;                /* 0x58e */
    u8  unk_58f;                /* 0x58f */
    u8  unk_590;                /* 0x590 */
    u8  unk_591;                /* 0x591 */
    u8  pad_592[0xa];
    /* Player * -- the ROM loads this WORD and passes it to _ZN6Player9DropActorEv as that
       function's `this`, which is an object address, so the word is a Player *. It says
       nothing about the rest of the marker's span, which stays explicit padding. Was a u8
       marker. */
    Player *unk_59c;            /* 0x59c */
    u8  pad_5a0[0x4];

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own.
       Nothing derives from SignPost, so no subclass can be made to emit a `bl`
       the ROM does not have -- but keeping it inline also keeps this class
       without a key function, so no translation unit that merely includes this
       header emits _ZTV8SignPost. The out-of-line D1 and D0 at ov002 0x020badd0
       and 0x020bae2c are still C translation units defining extern "C" free
       functions under the mangled names, exactly as include/dActor_c.h describes;
       neither defines SignPost::~SignPost. */
    virtual ~SignPost() {}

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). This class
       overrides it: _ZTV8SignPost (ov002 0x02109af8) carries 0x020bb3b8 at
       vtable + 0x7c where _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv, and slot 30
       is dActor_c's 0x020100dc in both. It adds no slot and no field, so the size
       assert below is unaffected.

       It is also this class's KEY FUNCTION, the destructor above being inline,
       so src/_ZN8SignPost4KillEv.cpp emits _ZTV8SignPost, _ZTI8SignPost and the
       destructor variants alongside the one function it is bound to.
       objisolate.py reduces the object back to that one 0x74 .text before
       eligible.py and rombuild.py judge it -- checked, not assumed. */
    virtual void Kill();        /* slot 31 */

    /* --- non-virtual --- */
    int CleanupResources();
    int InitResources();
    int Render();

    void OnGroundPounded(dActor_c &other);   /* slot 21 */
    int  OnAttacked1(dActor_c &other);       /* slot 22 */
    void OnHitByMegaChar(Player &player);    /* slot 27 */
};

typedef char SignPost_size_must_be_0x5a4[sizeof(SignPost) == 0x5a4 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit dBgActor_c's fields from and so spells the layout flat. Same
   arrangement as include/dBgActor_c.h. */
struct SignPost {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x24];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    u8  pad_092[0xa];
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0xc];
    s32 unk_0b0;            /* 0x0b0 */
    u8  pad_0b4[0x20];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    u8  unk_2ec;            /* 0x2ec */
    u8  pad_2ed[0x33];
    u8  mMovingCylinderClsn;            /* 0x320 */
    u8  pad_321[0x37];
    u8  mShadowModel;            /* 0x358 */
    u8  pad_359[0x57];
    s32 unk_3b0;            /* 0x3b0 */
    s32 unk_3b4;            /* 0x3b4 */
    s32 unk_3b8;            /* 0x3b8 */
    s16 unk_3bc;            /* 0x3bc */
    s16 unk_3be;            /* 0x3be */
    s16 unk_3c0;            /* 0x3c0 */
    u8  pad_3c2[0x6];
    u8  mWithMeshClsn;            /* 0x3c8 */
    u8  pad_3c9[0x1c5];
    u8  unk_58e;            /* 0x58e */
    u8  unk_58f;            /* 0x58f */
    u8  unk_590;            /* 0x590 */
    u8  unk_591;            /* 0x591 */
    u8  pad_592[0xa];
    Player *unk_59c;            /* 0x59c */
    u8  pad_5a0[0x4];
};

#endif /* __cplusplus */

#endif
