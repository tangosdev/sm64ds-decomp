#ifndef ANIMATION_H
#define ANIMATION_H

#include "types.h"
#include "math/Fix12.h"

/* The animation-playback root, vtable data_0208e7e4:
 *
 *   slot 0  0x02015ce8  ~Animation (D1)
 *   slot 1  0x02015cc4  ~Animation (D0)
 *   slot 2  0x00000000  pure virtual
 *
 * Slot 2 is null here and the MaterialChanger/TextureSequence family
 * overrides it with their Update(ModelComponents &); it keeps a neutral
 * name until that family converts and pins the signature. It is NOT
 * declared pure even though the slot is null, because ModelAnim2 embeds
 * a bare Animation as a data member.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h. C1/C2/D0/D1/D2 stay
 * self-contained translation units.
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
 * ModelAnim and its children carry an Animation as a SECOND base at
 * +0x50, reached through the ROM's _ZThn80_ thunks; that is the next
 * phase, not this header's problem.
 */

#ifdef __cplusplus

struct BCA_File;
struct SharedFilePtr;

struct Animation {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    u32 numFramesAndFlags;   /* 0x04 - count in bits 0..29, loop flags in 30..31 */
    Fix12i currFrame;        /* 0x08 - 20.12 */
    Fix12i speed;            /* 0x0c - 0x1000 is 1.0 */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~Animation();                /* slots 0 (D1), 1 (D0) */
    virtual void Virtual08();            /* slot 2 - null in the ROM table.
                                            Never defined anywhere; it reads
                                            as pure, but ModelAnim2 embeds a
                                            bare Animation member (built with
                                            C1), so it cannot be declared
                                            pure here. */

    /* --- non-virtual --- */
    void SetAnimation(u16 numFrames, int flags, Fix12<int> speed,
                      u16 startFrame);   /* defined as a free function, wall 6az */
    void Copy(const Animation &anim);
    void Advance();
    int Finished();
    int GetFlags();
    void SetFlags(int flags);
    u32 GetFrameCount() const;
    bool WillHitFrame(int frame) const;

    /* --- static --- */
    static char *LoadFile(SharedFilePtr &ptr);
    static void UpdateFileOffsets(BCA_File &file);
};

typedef char Animation_size_must_be_0x10[sizeof(Animation) == 0x10 ? 1 : -1];

#else

/* The same object for C translation units, vptr written out explicitly. */
struct Animation {
    void **vtable;             /* 0x00 */
    u32 numFramesAndFlags;     /* 0x04 */
    s32 currFrame;             /* 0x08 */
    s32 speed;                 /* 0x0c */
};

#endif /* __cplusplus */

#endif /* ANIMATION_H */
