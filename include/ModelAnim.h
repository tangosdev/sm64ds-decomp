#ifndef MODELANIM_H
#define MODELANIM_H

#include "types.h"
#include "BMD_File.h"
#include "ModelBase.h"
#include "Model.h"
#include "Animation.h"
#include "math/Matrix.h"
#include "math/Fix12.h"

/* The first multiply-inheriting class in the tree: ModelAnim derives from
 * Model AND Animation. The Animation base sits at +0x50, which is exactly
 * sizeof(Model), and the ROM's _ZThn80_ thunks (80 decimal = 0x50) adjust
 * this from the Animation view back to the object start.
 *
 * Primary vtable _ZTV9ModelAnim at 0x0208e980:
 *
 *   slot 0  0x0201691c  ~ModelAnim (D1)
 *   slot 1  0x020168d8  ~ModelAnim (D0)
 *   slot 2  0x02016bf8  Model::DoSetFile   -- INHERITED, deliberately not
 *                                             redeclared here
 *   slot 3  0x0201686c  UpdateVerts
 *   slot 4  0x0201682c  Virtual10(Matrix4x3 &)
 *   slot 5  0x020167f8  Render(Vector3 const *)
 *   slot 6  0x020167c4  Virtual18(u32, Vector3 const *)
 *
 * Secondary (Animation-in-ModelAnim) vtable at +0x24 of the primary,
 * VTable_Animation_ModelAnimThunk: [_ZThn80_ D1, _ZThn80_ D0, null] -- the
 * inherited slot stays null.
 *
 * CORRECTION. That null slot used to be read here as "ModelAnim is abstract,
 * which is why only its C2 is ever called from derived constructors and no
 * code instantiates one directly." The ROM refutes it, and not subtly: the
 * COMPLETE-OBJECT structors exist as symbols --
 *
 *   _ZN9ModelAnimC1Ev  0x02016958      _ZN9ModelAnimD1Ev  0x0201691c
 *
 * -- and are called from 103 and 131 source files respectively. A compiler
 * emits C1/D1 only for a type something actually creates; a class that can
 * only ever be a base gets C2/D2 alone. Those calls land at member offsets
 * inside other classes (Amp+0xd4, Flag+0xd4, QuestionSwitch+0x6b4, ...),
 * which is what an embedded member looks like, and they are relocations the
 * ROM build checks -- not prose.
 *
 * So a ModelAnim IS instantiated, overwhelmingly as a member subobject. The
 * null slot means one inherited virtual is never called, not that the class
 * cannot be built: it cannot mean abstract while C1 exists and 103 files
 * call it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD. Both
 * vtables AND the two thunks are emitted with the key function, so the
 * structors staying C files is what keeps all of them ROM-supplied. See
 * include/ModelBase.h.
 *
 * LAYOUT: Model 0x00..0x50, Animation 0x50..0x60 (its vptr at +0x50), the
 * BCA file pointer at 0x60. Pinned by C2 (both ctor calls, both vptr
 * stores, zeroes +0x60), D2 (both vptr restores, both base D2 calls) and
 * SetAnim/UpdateVerts reading +0x60.
 *
 * SetAnim's definition stays a mangled free function (wall 6az: its true
 * signature carries Fix12<int>); the declaration below is the real one.
 */

#ifdef __cplusplus

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct ModelAnim : Model, Animation {
    BCA_File *file;            /* 0x60 */

    /* --- primary vtable order. Do not reorder. --- */
    virtual ~ModelAnim();                                 /* slots 0 (D1), 1 (D0) */
    virtual void UpdateVerts();                           /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);               /* slot 4 */
    virtual void Render(const Vector3 *scale);            /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale);/* slot 6 */

    /* --- non-virtual --- */
    void SetAnim(BCA_File *animFile, int flags, Fix12<int> speed,
                 u32 startFrame);         /* defined as a free function, wall 6az */
    void Copy(const ModelAnim &src, char *newFile);

    /* ITS OWN, TO RESOLVE AN AMBIGUITY MULTIPLE INHERITANCE CREATES. ModelAnim
       derives from Model (so ModelBase) and from Animation, and both bases
       declare operator delete, so an inherited one is "ambiguous access to
       name found: ModelBase::operator delete and Animation::operator delete".
       Declaring it here picks the same deallocator both bases name, and also
       satisfies the rule in include/Actor.h that mwcc only inlines the member
       when it is in the class or its immediate base. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char ModelAnim_size_must_be_0x64[sizeof(ModelAnim) == 0x64 ? 1 : -1];

#else

/* The same object for C translation units, both vptrs written out. */
struct ModelAnim {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents data;       /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x1c */
    void *transformsBuf;               /* 0x4c */
    void **animVtable;                 /* 0x50 */
    u32 numFramesAndFlags;             /* 0x54 */
    s32 currFrame;                     /* 0x58 */
    s32 speed;                         /* 0x5c */
    struct BCA_File *file;             /* 0x60 */
};

/* So an object header declaring a ModelAnim member reads the same in both modes: C++
 * gets the class, C gets the flat stand-in above, and neither needs `struct`. */
typedef struct ModelAnim ModelAnim;

#endif /* __cplusplus */

#endif /* MODELANIM_H */
