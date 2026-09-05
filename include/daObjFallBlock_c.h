#ifndef DAOBJFALLBLOCK_C_H
#define DAOBJFALLBLOCK_C_H

#include "types.h"

/* The abstract base of the falling blocks: the platforms that shake for a moment
 * once you stand on them and then drop out from under you.
 *
 * A LAYER THE TREE DID NOT HAVE. This header used to be a flat struct under
 * `u8 pad_000[0x320]`, emitted by `tools/rtti_vtables.py --emit-headers` because
 * that pass knew offsets and not sizeof(base). include/dBgActor_c.h has since
 * settled sizeof(dBgActor_c) = 0x320, so the class can be spelled as what it is. The
 * regenerate line is gone with the generated body: that tool deletes only files
 * that still carry it, and this one is hand-written now.
 *
 *   _ZTI16daObjFallBlock_c  ov015 0x02114874
 *   _ZTS16daObjFallBlock_c  ov015 0x0211488c   "16daObjFallBlock_c"
 *   vtable                  ov098 0x0213c5bc, 32 slots, same count as the base
 *   kind                    __si_class_type_info, ONE base, subobject offset 0
 *   base                    dBgActor_c, ov002 0x021089ec -- the tree's dBgActor_c
 *
 * THE VTABLE LIVES IN ov098, WITH THE CODE, and the typeinfo record does not.
 * Every one of this class's own slots resolves to an ov098 function, which is how
 * the shared behaviour survives a level change while each leaf sits in its own
 * level overlay. The relocations that reach the vtable carry the ambiguous tag
 * `overlays(6,98)` because that pair shares address space; the config settles it
 * from the other side by marking ov006's copy of 0x0213c5bc `ambiguous` and
 * leaving ov098's as the definition.
 *
 * ABSTRACT. Slots 0 and 3 -- InitResources and CleanupResources -- are null. Its
 * own overrides are 6 (Behavior), 9 (Render), 16 (D1), 17 (D0), 27
 * (OnHitByMegaChar) and 31 (Kill).
 *
 * FOUR DESCENDANTS, one per level overlay: daObjBk_Fall_Block_c (daObjBk_Fall_Block_c,
 * ov015), daObjFl_Fall_Block_c (ov022, only ever named by its factory
 * daObjFl_Fall_Block_c_classInit), daObjKm2_Fall_Block_c (FallBlockBfs, ov045) and
 * daObjTh_Fall_Block_c (FallBlockBbh, ov063). Each one's destructor stores this
 * class's vtable between its own and _ZTV10dBgActor_c.
 *
 * SIZE 0x34c, the literal all four factories pass to fBase_c::operator new.
 *
 * THE FIELDS COME OUT OF ONE FUNCTION, `_ZN16daObjFallBlock_c8BehaviorEv`,
 * which is this class's own slot 6 and lives in this class's own overlay. It
 * is the whole fall-block state machine and it reaches every one of them:
 *
 *   0x320  the rest position, a Vector3. Case 3 passes `this+0x320` to
 *          Vec3_HorzDist against the closest player, and 0x320 + 12 = 0x32c lands
 *          exactly on the next field.
 *   0x32c  the Y below which the block is gone. Case 2 compares the actor's own Y
 *          at 0x60 against it and calls slot 31 when it drops under.
 *   0x330  which of the two shake routines case 0 runs.
 *   0x334  the shake step added to mAngleX each frame in case 2.
 *   0x336  the same for mAngleY.
 *   0x338  the fall speed, stepped from a sine table in case 1.
 *   0x33a  the per-state frame counter, walked down by DecIfAbove0_Short.
 *   0x33c  the state, 0..3 -- the switch this whole function is built on.
 *   0x33e  set when case 0 should start shaking.
 *   0x33f  the respawn delay. While it runs the collider is disabled and Behavior
 *          returns early.
 *   0x340  this block's "ready" flag, and the one the group walk tests.
 *   0x342  suppresses Render as well as Behavior, for actor id 0x53.
 *   0x344  non-zero means this block is not the head of a group.
 *   0x348  the next block in the group. Behavior walks the chain twice through it,
 *          once to see whether every member's 0x340 is set and once to trip them
 *          all together.
 *
 * 0x348 + 4 = 0x34c closes the class on the factories' literal with no gap left in
 * the span. The generated body had 0x320..0x344 and stopped there; the Vector3 and
 * the group link are new. Field NAMES are coined from what the code does; nothing
 * in the ROM names them.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjFallBlock_c : dBgActor_c {
    /* Field NAMES are placeholders. Offsets, widths and types are observed. */
    Vector3 mRestPos;                   /* 0x320 */
    s32  mKillY;                        /* 0x32c */
    s32  mShakeKind;                    /* 0x330 */
    s16  mShakeX;                       /* 0x334 */
    s16  mShakeY;                       /* 0x336 */
    s16  mFallSpeed;                    /* 0x338 */
    u16  mStateTimer;                   /* 0x33a */
    u8   mState;                        /* 0x33c */
    u8   pad_33d[0x1];
    u8   mShakeRequested;               /* 0x33e */
    u8   mRespawnDelay;                 /* 0x33f */
    u8   mReady;                        /* 0x340 */
    u8   pad_341[0x1];
    u8   mSuppressed;                   /* 0x342 */
    u8   pad_343[0x1];
    s32  mNotGroupHead;                 /* 0x344 */
    daObjFallBlock_c *mNextInGroup;     /* 0x348 */

    /* --- vtable --- */
    /* INLINE ON PURPOSE, for the reason include/dBgActor_c.h gives for its own:
       every descendant's destructor inlines this body rather than calling
       _ZN16daObjFallBlock_cD1Ev (which does exist out of line, at ov098
       0x02139fc8). An out-of-line declaration here would make each descendant
       emit a `bl` the ROM does not have.

       MEASURED, not assumed. config/arm9/overlays/ov098/relocs.txt holds exactly
       two inbound references to the destructor pair:

         from:0x0213c5fc kind:load to:0x02139fc8   _ZTV16daObjFallBlock_c + 0x40
         from:0x0213c600 kind:load to:0x02139f70   _ZTV16daObjFallBlock_c + 0x44

       -- vtable slots 16 and 17, the two this header already names. No
       instruction anywhere in the image calls either address, and no
       _ZN16daObjFallBlock_cD2Ev exists in the image or in any symbols.txt; an
       out-of-line base destructor would make all four descendants call D2 and
       would leave it live. The in-class body is the cartridge's form.

       IT COSTS THE TU PROMOTION, AND THAT IS THE RIGHT TRADE. An in-class inline
       destructor makes mwccarm emit D1 before D0, and the ROM lays them down the
       other way round (D0 0x02139f70, D1 0x02139fc8). So the merged TU in
       src_tu/actors/daObjFallBlock_c.cpp reproduces all twelve bodies
       byte-for-byte but cannot put its licensed .text in ROM order -- see that
       file and config/tu_manifest.d/ov098/daObjFallBlock_c.json. Do not "fix"
       the ordering by moving this body out of line: it trades four descendants'
       bytes for one TU's. */
    virtual ~daObjFallBlock_c() {}

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). This class
       overrides it; it adds no slot and no field, so the size assert below is
       unaffected.

       It is also this class's KEY FUNCTION, the destructor above being inline,
       so src/_ZN16daObjFallBlock_c4KillEv.cpp emits _ZTV16daObjFallBlock_c,
       _ZTI16daObjFallBlock_c and the destructor variants alongside the one
       function it is bound to. objisolate.py reduces the object back to that
       one 0xc0 .text before eligible.py and rombuild.py judge it -- checked,
       not assumed: build_pin.verify returns True either way. */
    virtual void Kill();                /* slot 31 */

    /* Slot 6, this class's own override -- the whole fall-block state
       machine. Not marked `virtual` here: fBase_c already declares it
       virtual, and the override keyword isn't required to re-add it. */
    s32 Behavior();                     /* slot  6 */
    s32 Render();                       /* slot  9 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    /* THE NULL SLOTS THE NOTE ABOVE ALREADY NAMES, SPELT SO THE COMPILER AGREES.
       mwccarm lays down a bare 0x00000000 with no relocation for a pure virtual --
       there is no __cxa_pure_virtual in this image for it to point at -- so a zero
       word in a ROM vtable IS the `= 0`, and it is the only thing that produces one.
       Left undeclared, this class silently inherits dBgActor_c's concrete bodies and
       the vtable it emits disagrees with the cartridge at exactly these slots.
       Measured by tools/romdata_check.py, which is the only gate that reads them:
       the ROM build's 106/106 compares .text alone and is blind here.

       DECLARED LAST, AND WITH `virtual` -- unlike the plain overrides above. The
       pure-specifier is only valid on a declaration carrying the keyword, and a
       pure virtual has no body to emit, so it can never become the key function:
       whichever virtual was first and non-inline before is still first now. */
    virtual s32 InitResources() = 0;        /* slot  0 */
    virtual s32 CleanupResources() = 0;     /* slot  3 */
};

typedef char daObjFallBlock_c_size_must_be_0x34c[sizeof(daObjFallBlock_c) == 0x34c ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base sub-object to
   inherit dBgActor_c's fields from and so spells the layout flat. Same arrangement
   as include/dBgActor_c.h. */
struct daObjFallBlock_c {
    u8  pad_000[0x320];
    s32 mRestPosX;          /* 0x320 */
    s32 mRestPosY;          /* 0x324 */
    s32 mRestPosZ;          /* 0x328 */
    s32 mKillY;             /* 0x32c */
    s32 mShakeKind;         /* 0x330 */
    s16 mShakeX;            /* 0x334 */
    s16 mShakeY;            /* 0x336 */
    s16 mFallSpeed;         /* 0x338 */
    u16 mStateTimer;        /* 0x33a */
    u8  mState;             /* 0x33c */
    u8  pad_33d[0x1];
    u8  mShakeRequested;    /* 0x33e */
    u8  mRespawnDelay;      /* 0x33f */
    u8  mReady;             /* 0x340 */
    u8  pad_341[0x1];
    u8  mSuppressed;        /* 0x342 */
    u8  pad_343[0x1];
    s32 mNotGroupHead;      /* 0x344 */
    struct daObjFallBlock_c *mNextInGroup;      /* 0x348 */
};

#endif /* __cplusplus */

#endif /* DAOBJFALLBLOCK_C_H */
