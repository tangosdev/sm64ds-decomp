/* Seeded from matched-function evidence by tools/gen_header.py, then given its
 * real base and real member types by hand.
 *
 * class dBgActor_c: 9 matched functions. The base of a large family -- ArmedRotatingPlatform,
 * PyramidTop, daObjKm3_Kaitendai_c, ShutterBob, ShutterHmc, SlidingIce and others -- whose
 * destructors all inline this one's body, which is why it is declared inline
 * below.
 *
 * Two sub-objects, each checked twice -- once by that class's own size assertion,
 * once by closing exactly on the next named field:
 *
 *     dActor_c               0x000 + 0x0d0 = 0x0d0   -> pad_0d0
 *     Model               0x0d4 + 0x050 = 0x124   -> mMeshCollider
 *     dBgW_KcMbg  0x124 + 0x1c8 = 0x2ec   -> padding, then unk_310
 *
 * FOUR OF THE GENERATED HEADER'S FIELDS WERE THE Model'S OWN INSIDES and are gone
 * from this half: unk_0f0 (0x0d4 + 0x1c) and unk_114 / unk_118 / unk_11c
 * (0x0d4 + 0x40 / +0x44 / +0x48). They were declared as siblings of a `u8 mModel`
 * marker whose pad stopped short of the real object.
 *
 * SIZEOF IS 0x320, and it takes more than one derived class to see why.
 * BowserFireSeaArena starts its own Model at 0x324, which alone reads like the
 * class ending there. But FOUR classes derive from dBgActor_c DIRECTLY -- one
 * non-dBgActor_c vtable store each, so no intermediate -- and each places a
 * 4-byte-aligned CLASS member at 0x320, impossible if this class occupied
 * 0x320..0x323:
 *
 *     PyramidTop      daObjDlPyramid_c   Model                      @ 0x320
 *     BasementWater    daObjC0Water_c     TextureTransformer         @ 0x320
 *     TowerStep  daObjBk_Lift_c     ShadowModel                @ 0x320
 *     WallSign        daObjKanban_c      dCcAcPos_c  @ 0x320
 *
 * Each is read straight off that class's destructor, which destroys its own
 * member at 0x320 before storing _ZTV10dBgActor_c and running the base.
 *
 * One layout satisfies all five: data ends 0x31e, sizeof 0x320. The four above
 * align up from 0x31e to 0x320; BowserFireSeaArena puts its OWN three s16 at
 * 0x31e/0x320/0x322 so its Model lands at 0x324; ArmedRotatingPlatform puts a single s16
 * at 0x31e, in this class's tail padding, and its Behavior reads this+0x31e
 * and reproduces. Ending at 0x324 satisfies BowserFireSeaArena and contradicts
 * the other four.
 *
 * Field NAMES for the unk_ entries are placeholders. */
#ifndef DBGACTOR_C_H
#define DBGACTOR_C_H
#include "types.h"

/* fwd */
struct Player;
struct player_;
#ifdef __cplusplus

/* FIRST, AND THE ORDER MATTERS. Matrix4x3 has two guarded spellings that share the
   0x30 bytes -- common.h's flat `s32 m[12]` and math/Matrix.h's `{Matrix3x3 r;
   Vector3 t;}` -- and whichever a translation unit sees first stands (see the note
   in common.h). Model.h pulls in the second one, so without this line dBgActor_c's
   TUs would get `.r`/`.t`.

   The ROM says it was the flat one here: UpdateClsnPosAndRot copies mModel.mat4x3
   into mClsnMat as THREE ldm/stm pairs of four registers -- twelve flat words. The
   `{r, t}` spelling copies the two members separately, 9 words then 3, and the
   function comes out 0x74 against the ROM's 0x64. Both were built. */
#include "common.h"

#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

struct dBgActor_c : dActor_c {
    u8  pad_0d0[0x4];
    /* Named by the class's own destructor calling Model's D1 at +0x0d4 and
       dBgW_KcMbg's at +0x124 -- relocations the ROM build checks. */
    Model mModel;                           /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;       /* 0x124 */
    /* 0x2ec: a Matrix4x3, and the generated header's unk_310 / unk_314 / unk_318
       were its translation row -- m[9], m[10], m[11], at +0x24/+0x28/+0x2c.
       UpdateClsnPosAndRot copies mModel.mat4x3 in here and then overwrites exactly
       those three words with the actor's position, which is what a
       position-and-rotation matrix update IS. Naming them separately described the
       same bytes twice. */
    Matrix4x3 mClsnMat;     /* 0x2ec */
    u8  unk_31c;            /* 0x31c */
    u8  unk_31d;            /* 0x31d */
    /* THE CLASS ENDS HERE, at 0x31e; sizeof rounds to 0x320. See the header
       comment: three s16 used to sit here, and they are BowserFireSeaArena's. */

    /* --- vtable, in ROM order. Do not reorder. --- */
    /* INLINE ON PURPOSE. Every dBgActor_c subclass's destructor inlines this body
       rather than calling _ZN10dBgActor_cD1Ev (which does exist, at ov002
       0x020ee42c, for the times it is called out of line). An out-of-line
       declaration here would make each subclass emit a `bl` the ROM does not
       have. Being inline also keeps dBgActor_c without a key function, so no
       translation unit that merely includes this header emits _ZTV10dBgActor_c. */
    virtual ~dBgActor_c() {}

    /* SLOT 31, AND THE ONLY NEW VIRTUAL THIS CLASS ADDS. dActor_c ends at slot 30,
       so this one word is the whole difference between the vtable this header
       emitted and the one in the cartridge.

       _ZTV10dBgActor_c is 0x84 at ov002:0x0210ae38 -- 33 words, one more than the
       32 an dActor_c-shaped table needs -- and _ZTV8PoleLift, one of the 70
       subclasses, is 0x84 as well. rtti_vtables agrees from the other side:
       dBgActor_c's parent dActor_c has 31 slots and dBgActor_c's own overrides
       are 16 (D1), 17 (D0) and 31, and 97 of its 101 RTTI children have exactly
       32 slots. Without this line every one of those tables came out a word
       short, which is why daObjFallBlock_c::Kill and its siblings had to reach
       slot 31 through a hand-declared 32-slot shadow struct.

       Kill is not an override: no ancestor declares it, so `virtual` here
       CREATES the slot. It also makes Kill this class's key function -- the
       destructor above is inline on purpose -- so the TU defining it emits
       _ZTV10dBgActor_c, _ZTI10dBgActor_c and the destructor variants alongside. That
       is expected and handled: objisolate.py reduces the object to the one
       function its delink entry declares before eligible.py and rombuild.py
       judge it. */
    virtual void Kill();

    /* --- non-virtual --- */
    void KillByMegaChar(Player &player_);
    void UpdateClsnPosAndRot();
    void UpdateModelPosAndRotY();

    /* BY-VALUE Fix12<int> HERE IS MEASURED, NOT INFERRED FROM THE NAME, and the
       distinction matters -- the cartridge's RTTI carries class names only, so
       every parameter type in a mangled symbol is this project's reconstruction
       until some function's bytes test it. This one is tested: the body at ov002
       0x020ee674 reproduces with a single-int-by-value fourth parameter, which
       is exactly what math/Fix12.h's `{ T val; }` is. Contrast the two
       IsClsnInRange* symbols, whose names make the same Fix12<int> claim and
       whose bytes REFUSE it -- see the note in include/dActor_c.h about CW homing
       class-typed by-value parameters to the stack, and the header comment in
       src_tu/actors/dBgActor_c.cpp.

       Non-virtual, so this declaration adds no slot and no field; the 0x320
       assertion below is unaffected. */
    int UpdateKillByMegaChar(s16 a, s16 b, s16 c, Fix12<int> d);
};

typedef char dBgActor_c_size_must_be_0x320[sizeof(dBgActor_c) == 0x320 ? 1 : -1];

#else

/* The same object for a C translation unit, which has no base class to inherit
   dActor_c's fields from and so spells the whole layout flat. */
struct dBgActor_c {
    u8  pad_000[0x5c];
    s32 mPosX;            /* 0x05c */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0xc];
    /* 0x074..0x08e is dActor_c's, and dActor_c.h is de-bannered -- hand-reconstructed, not generated. Was one u8
       marker over the whole range. */
    s32 unk_074;                 /* 0x074 */
    s32 mCamSpacePosY;           /* 0x078 */
    s32 mCamSpacePosZ;           /* 0x07c */
    s32 mScaleX;                 /* 0x080 */
    s32 mScaleY;                 /* 0x084 */
    s32 mScaleZ;                 /* 0x088 */
    s16 mAngleX;                 /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x4];
    s16 mPrevAngleY;            /* 0x094 */
    u8  pad_096[0x2];
    s32 unk_098;            /* 0x098 */
    s32 unk_09c;            /* 0x09c */
    s32 unk_0a0;            /* 0x0a0 */
    u8  pad_0a4[0x4];
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x4];
    s32 unk_0b0;            /* 0x0b0 */
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    u8  pad_0bc[0x18];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x1b];
    u8  unk_0f0;            /* 0x0f0 */
    u8  pad_0f1[0x23];
    s32 unk_114;            /* 0x114 */
    s32 unk_118;            /* 0x118 */
    s32 unk_11c;            /* 0x11c */
    u8  pad_120[0x4];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    struct Matrix4x3 mClsnMat;    /* 0x2ec */
    u8  unk_31c;            /* 0x31c */
    u8  unk_31d;            /* 0x31d */
    /* Ends at 0x31e; sizeof rounds to 0x320. See the C++ half. */
};

#endif /* __cplusplus */


#endif
