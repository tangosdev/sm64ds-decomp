/* class cMgSmartball_object_c, real ROM name confirmed by
 * tools/rtti_extract.py (build/rtti.json). Own vtable ov006:0x0213ed10.
 *
 * A ROOT. It has no RTTI base at all -- this is a separate hierarchy from
 * the fBase_c/dScene_c tree that the rest of this directory models, and the
 * first class in it anyone has touched. Eleven direct children:
 * ball, pakkun, dokan, kinoko, pushswitch, propeller, wing, ana, board,
 * spring, slot.
 *
 * NO VIRTUAL DESTRUCTOR. Its vtable is exactly THREE slots and none of them
 * is a structor -- all three are ordinary virtual methods. That makes this
 * family a different shape from every other class in this tree, where the
 * destructor pair is what gets migrated first. There is nothing to migrate
 * here but real methods, and no operator delete is involved.
 *
 * CONSTRUCTED BY func_ov006_0211474c, which is this class's C1/C2 in all
 * but name: it writes this vtable and then every field below. It is left as
 * a free function -- this tree has migrated zero constructors (langmode
 * audit: "C1: UNPROVEN, 0 migrated"), and proving that route is its own
 * piece of work, not a rider on this one.
 *
 * SIZE 0x34. The constructor's last write is the u8 at 0x30, which alone
 * would only give 0x31 rounded to 0x34; the upper bound comes from a child,
 * whose own constructor writes 0x34 and 0x4c. Base ends where the first
 * derived field begins.
 *
 * THE THREE 2-WORD PAIRS ARE THE INTERESTING PART, and their RELATIONSHIP
 * is what is evidenced, not their meaning. The constructor writes the same
 * incoming pair to BOTH 0x08 and 0x18. Slot 0 copies 0x08/0x0c -> 0x10/0x14.
 * Slot 2 copies 0x18/0x1c -> 0x08/0x0c. So 0x18 holds a value that 0x08 can
 * be restored from, and 0x10 holds a snapshot of 0x08. The field names below
 * say that much and no more.
 *
 * METHOD NAMES ARE COINED FROM THE BODIES, not recovered from the ROM.
 * Nothing in this hierarchy has a named slot map to inherit from -- it is a
 * root, so there is no base whose names could settle them. Each name below
 * describes exactly what its body does; none of them is a claim about what
 * the original called it. Renaming later costs one commit. */
#ifndef CMGSMARTBALL_OBJECT_C_H
#define CMGSMARTBALL_OBJECT_C_H
#include "types.h"

struct cMgSmartball_object_c {
    virtual void SaveSnapshot();   /* slot 0 -- mSnapshot = mCurrent */
    virtual void Update();         /* slot 1 -- empty in this class */
    virtual void RestoreInitial(); /* slot 2 -- mCurrent = mInitial */

    /* The C spelling of this class gets no explicit vtable member because
       no .c file declares it; every reader reaches these fields by raw
       offset today. */
    s32 mpManager;    /* 0x004 -- the constructor's first argument: the
                          dScMgSmartball_c that owns this object. Every child
                          dereferences it as that manager (mgr+0x4664 the
                          index of the ball in play, mgr+0x4668/0x4688 the
                          tracked-ball count and table, mgr+0x4670/0x46bc the
                          kinoko count and table, mgr+0x595c/0x595d two pause
                          flags). DECLARED s32, NOT A POINTER TYPE, because
                          the manager has no header here and every reader
                          spells its own cast -- the name states what the word
                          is, the type states nothing it cannot back up. */
    s32 mCurrent0;    /* 0x008 -- restored from mInitial by RestoreInitial */
    s32 mCurrent1;    /* 0x00c */
    s32 mSnapshot0;   /* 0x010 -- written from mCurrent by SaveSnapshot */
    s32 mSnapshot1;   /* 0x014 */
    s32 mInitial0;    /* 0x018 -- constructor writes the same pair here and
                          at mCurrent */
    s32 mInitial1;    /* 0x01c */
    s32 mVel0;        /* 0x020 -- constructor zeroes it. Velocity, x half:
                          cMgSmartball_ball_c::SaveSnapshot does
                          `mCurrent0 += mVel0; mCurrent1 += mVel1;` and passes
                          &mVel0 to Vec2_Len as a 2-vector. */
    s32 mVel1;        /* 0x024 -- constructor zeroes it; the z half of that
                          pair, and the value cMgSmartball_spring_c eases
                          mCurrent1 back to rest with. */
    s32 mRadius;      /* 0x028 -- constructor zeroes it and each child's own
                          constructor then writes its own constant (0x8000
                          ball, 0x14000 pakkun, 0x18000 board, 0x20000 dokan/
                          propeller/spring, 0x7000 kinoko). Both readers use it
                          as a distance: cMgSmartball_ball_c::SaveSnapshot
                          compares a signed plane distance against -mRadius,
                          and cMgSmartball_kinoko_c eases it to 0 and back to
                          0x7000 as the mushroom squashes, with that class's
                          Update turning it into a render scale. Ten other
                          files in this family read or write it, the
                          most-touched field of the class. */
    s32 mIndex;       /* 0x02c -- the constructor's second argument: this
                          object's slot in the manager's tables.
                          func_ov006_021128fc compares it against mgr+0x4664
                          and indexes mgr+0x4688 with it minus one. */
    u8  mIsActive;    /* 0x030 -- constructor sets it to 1. While it is 0
                          cMgSmartball_ball_c's SaveSnapshot and Update return
                          immediately, func_ov006_02111dcc refuses to arm an
                          expiry, and both cMgSmartball_board_c::SaveSnapshot
                          and cMgSmartball_kinoko_c::SaveSnapshot skip a
                          tracked ball whose byte at 0x30 is 0. */
    /* 0x031-0x033 IS A THREE-BYTE REGION WITH NO SINGLE TYPE, and that is the
       answer, not a gap in the evidence. Three children read the same three
       bytes three incompatible ways, all of them byte-matching:

         - cMgSmartball_ana_c writes 0x031 and 0x032 as independent u8 0/1
           flags and never touches 0x033.
         - cMgSmartball_wing_c and cMgSmartball_propeller_c read a 16-bit
           angle spanning 0x032-0x033 -- wing signed (compares < 0x3000,
           += 0x200), propeller unsigned (>> 4 to index a sine table, lsr not
           asr, a real codegen difference). They disagree with each other, so
           no one signedness is right for both.
         - cMgSmartball_board_c treats 0x031 as the HEAD of a nine-element
           per-cell byte array that runs to 0x039 -- i.e. through all three
           of these bytes and on into the derived class.

       An earlier revision declared 0x032 as a single s16 and called the
       signedness "contested". It is not contested; it is per-reader. Three
       u8s is the declaration that lets each reader spell its own access
       correctly with a cast, and misstates none of them. Every caller reaches
       these bytes by raw cast today, so this is byte-free -- but a future
       caller that wants the 16-bit angle must cast, not add a name. */
    u8  unk_031;      /* 0x031 -- a 0/1 flag; 11 files read or write it. Also
                          the first cell of cMgSmartball_board_c's nine-byte
                          cell array. */
    u8  unk_032;      /* 0x032 -- low byte of the wing/propeller angle, an
                          independent flag to ana, a board cell to board.
                          43 files touch this span. */
    u8  unk_033;      /* 0x033 -- high byte of that angle; a board cell. */
};

typedef char cMgSmartball_object_c_size_must_be_0x34[sizeof(cMgSmartball_object_c) == 0x34 ? 1 : -1];

#endif
