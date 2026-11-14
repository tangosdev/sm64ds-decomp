#ifndef DEXTFRAMECTRL_C_H
#define DEXTFRAMECTRL_C_H

#include "types.h"
#include "math/Fix12.h"

/* The animation-playback root, vtable _ZTV15dExtFrameCtrl_c:
 *
 *   slot 0  0x02015ce8  ~dExtFrameCtrl_c (D1)
 *   slot 1  0x02015cc4  ~dExtFrameCtrl_c (D0)
 *
 * TWO slots, nothing else. The zero word after them is adjacent data,
 * not a pure-virtual slot -- it is the offset-to-top word of the vtable
 * that follows, and all three _ZThn80_ secondary-base tables are likewise
 * two slots wide, which a derived table could not be if a third virtual
 * existed. The family's Update(ModelComponents&) is a plain method, not
 * an override. notes/model-rtti-names.md walks the pointers.
 *
 * The ROM's RTTI names this class dExtFrameCtrl_c -- a frame controller,
 * i.e. the playback cursor, not the animation data.
 *
 * The destructor is declared first, which makes it the key virtual function.
 * src/engine/model/d_ext_model.cpp defines it once, and mwcc emits
 * D2/D0/D1 together along with the class's _ZTV/_ZTI/_ZTS.
 *
 * LAYOUT is 0x10 bytes, pinned by C2 (vptr store, +0x8 zeroed, +0xc set
 * to 0x1000 = 1.0) and read consistently by Copy/Advance/Finished:
 * frame count in the low 30 bits of +0x4 with the loop flags in the top
 * two, current frame at +0x8 as 20.12, playback speed at +0xc.
 *
 * SetAnimation is a real method. Its symbol is
 * _ZN15dExtFrameCtrl_c12SetAnimationEtiit: speed is the scalar fx32,
 * which mangles as i, not the by-value Fix12<int> the old name invented.
 * The body reads it straight from a register (notes/mwccarm-codegen.md
 * 6az, MEASURED 2026-10-22; the caller does not pay either, 2026-10-23).
 *
 * dExtAnmModel_c and its children carry an dExtFrameCtrl_c as a SECOND base at
 * +0x50, reached through the ROM's _ZThn80_ thunks; that is the next
 * phase, not this header's problem.
 */

struct BCA_File;
struct SharedFilePtr;

struct dExtFrameCtrl_c {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u32 numFramesAndFlags;   /* 0x04 - count in bits 0..29, loop flags in 30..31 */
    fx32 currFrame;          /* 0x08 - 20.12 */
    fx32 speed;              /* 0x0c - 0x1000 is 1.0 */

    /* --- vtable, in ROM order: the destructor pair and nothing else. --- */
    virtual ~dExtFrameCtrl_c();                /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined here -- src/engine/model/d_ext_model.cpp
       defines it out of line and emits both C1 and the base-subobject C2
       (notes/ctor-migration.md section 2). Declaring it is what makes
       dExtAnmMaterial_c/dExtAnmTexSRT_c/dExtAnmTexPat_c (and dExtAnmModel_c's
       base step) emit `bl _ZN15dExtFrameCtrl_cC2Ev' instead of synthesising inline. */
    dExtFrameCtrl_c();

    /* --- non-virtual --- */
    void SetAnimation(u16 numFrames, int flags, fx32 speed, u16 startFrame);
    void Copy(const dExtFrameCtrl_c &anim);
    void Advance();
    int Finished();
    int GetFlags();
    void SetFlags(int flags);
    u32 GetFrameCount() const;
    bool WillHitFrame(int frame) const;

    /* --- static --- */
    static BCA_File *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(BCA_File &file);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtFrameCtrl_c_size_must_be_0x10[sizeof(dExtFrameCtrl_c) == 0x10 ? 1 : -1];
#endif

#endif /* DEXTFRAMECTRL_C_H */
