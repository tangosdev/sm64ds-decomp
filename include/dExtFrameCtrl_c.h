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
 * src/engine/animation/d_ext_frame_ctrl.cpp defines it once, and mwcc emits
 * D2/D0/D1 together along with the class's _ZTV/_ZTI/_ZTS.
 *
 * LAYOUT is 0x10 bytes, pinned by C2 (vptr store, +0x8 zeroed, +0xc set
 * to 0x1000 = 1.0) and read consistently by Copy/Advance/Finished:
 * frame count in the low 30 bits of +0x4 with the loop flags in the top
 * two, current frame at +0x8 as 20.12, playback speed at +0xc.
 *
 * SetAnimation's definition stays a mangled free function: its true
 * signature carries Fix12<int> and wall 6az (notes/mwccarm-codegen.md)
 * homes class-typed by-value parameters. The declaration below is the
 * real one and callers may use it.
 *
 * ModelAnim and its children carry an dExtFrameCtrl_c as a SECOND base at
 * +0x50, reached through the ROM's _ZThn80_ thunks; that is the next
 * phase, not this header's problem.
 */

#ifdef __cplusplus

struct BCA_File;
struct SharedFilePtr;

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct dExtFrameCtrl_c {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u32 numFramesAndFlags;   /* 0x04 - count in bits 0..29, loop flags in 30..31 */
    Fix12i currFrame;        /* 0x08 - 20.12 */
    Fix12i speed;            /* 0x0c - 0x1000 is 1.0 */

    /* --- vtable, in ROM order: the destructor pair and nothing else. --- */
    virtual ~dExtFrameCtrl_c();                /* slots 0 (D1), 1 (D0) */

    /* DECLARED, never defined here -- src/engine/animation/d_ext_frame_ctrl.cpp
       defines it out of line and emits both C1 and the base-subobject C2
       (notes/ctor-migration.md section 2). Declaring it is what makes
       MaterialChanger/TextureTransformer/TextureSequence (and ModelAnim's
       base step) emit `bl _ZN15dExtFrameCtrl_cC2Ev' instead of synthesising inline. */
    dExtFrameCtrl_c();

    /* --- non-virtual --- */
    void SetAnimation(u16 numFrames, int flags, Fix12<int> speed,
                      u16 startFrame);   /* defined as a free function, wall 6az */
    void Copy(const dExtFrameCtrl_c &anim);
    void Advance();
    int Finished();
    int GetFlags();
    void SetFlags(int flags);
    u32 GetFrameCount() const;
    bool WillHitFrame(int frame) const;

    /* --- static --- */
    static char *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(BCA_File &file);

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. dActor_c's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/dActor_c.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtFrameCtrl_c_size_must_be_0x10[sizeof(dExtFrameCtrl_c) == 0x10 ? 1 : -1];
#endif

#else

/* The same object for C translation units, vptr written out explicitly. */
struct dExtFrameCtrl_c {
    void **vtable;             /* 0x00 */
    u32 numFramesAndFlags;     /* 0x04 */
    s32 currFrame;             /* 0x08 */
    s32 speed;                 /* 0x0c */
};

#endif /* __cplusplus */

#endif /* DEXTFRAMECTRL_C_H */
