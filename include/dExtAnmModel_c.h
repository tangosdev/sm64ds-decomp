#ifndef DEXTANMMODEL_C_H
#define DEXTANMMODEL_C_H

#include "types.h"
#include "BMD_File.h"
#include "dExtModel_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtFrameCtrl_c.h"
#include "math/Matrix.h"
#include "math/Fix12.h"

/* The first multiply-inheriting class in the tree: dExtAnmModel_c derives from
 * dExtSimpleModel_c AND dExtFrameCtrl_c. The dExtFrameCtrl_c base sits at +0x50, which is exactly
 * sizeof(dExtSimpleModel_c), and the ROM's _ZThn80_ thunks (80 decimal = 0x50) adjust
 * this from the dExtFrameCtrl_c view back to the object start.
 *
 * Primary vtable _ZTV14dExtAnmModel_c at 0x0208e980:
 *
 *   slot 0  0x0201691c  ~dExtAnmModel_c (D1)
 *   slot 1  0x020168d8  ~dExtAnmModel_c (D0)
 *   slot 2  0x02016bf8  dExtSimpleModel_c::DoSetFile   -- INHERITED, deliberately not
 *                                             redeclared here
 *   slot 3  0x0201686c  UpdateVerts
 *   slot 4  0x0201682c  Virtual10(Matrix4x3 &)
 *   slot 5  0x020167f8  Render(Vector3 const *)
 *   slot 6  0x020167c4  Virtual18(u32, Vector3 const *)
 *
 * Secondary (dExtFrameCtrl_c-in-dExtAnmModel_c) vtable at _ZTV14dExtAnmModel_c
 * + 0x24, inside the same vtable group (preamble -0x50 / _ZTI14dExtAnmModel_c,
 * then [_ZThn80_ D1, _ZThn80_ D0, null]) -- the inherited slot stays null. The
 * cartridge gives the secondary table no name; config/arm9/symbols.txt labels it
 * VTable_dExtFrameCtrl_c_dExtAnmModel_cThunk because dsd needs a symbol at
 * every relocation target.
 *
 * CORRECTION. That null slot used to be read here as "dExtAnmModel_c is abstract,
 * which is why only its C2 is ever called from derived constructors and no
 * code instantiates one directly." The ROM refutes it, and not subtly: the
 * COMPLETE-OBJECT structors exist as symbols --
 *
 *   _ZN14dExtAnmModel_cC1Ev  0x02016958      _ZN14dExtAnmModel_cD1Ev  0x0201691c
 *
 * -- and are called from 103 and 131 source files respectively. A compiler
 * emits C1/D1 only for a type something actually creates; a class that can
 * only ever be a base gets C2/D2 alone. Those calls land at member offsets
 * inside other classes (Amp+0xd4, daMcFlag_c+0xd4, QuestionSwitch+0x6b4, ...),
 * which is what an embedded member looks like, and they are relocations the
 * ROM build checks -- not prose.
 *
 * So a dExtAnmModel_c IS instantiated, overwhelmingly as a member subobject. The
 * null slot means one inherited virtual is never called, not that the class
 * cannot be built: it cannot mean abstract while C1 exists and 103 files
 * call it.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD. Both
 * vtables AND the two thunks are emitted with the key function, so the
 * structors staying C files is what keeps all of them ROM-supplied. See
 * include/dExtModel_c.h.
 *
 * LAYOUT: dExtSimpleModel_c 0x00..0x50, dExtFrameCtrl_c 0x50..0x60 (its vptr at +0x50), the
 * BCA file pointer at 0x60. Pinned by C2 (both ctor calls, both vptr
 * stores, zeroes +0x60), D2 (both vptr restores, both base D2 calls) and
 * SetAnim/UpdateVerts reading +0x60.
 *
 * SetAnim IS A REAL METHOD, and its symbol is _ZN14dExtAnmModel_c7SetAnimEP8BCA_Fileiit.
 * It used to be a mangled extern "C" free function taking an explicit `thiz`,
 * on the grounds that wall 6az forbade the method form. That was wrong: the
 * wall was the NAME. The old symbol spelled a by-value `Fix12<int>` that we
 * invented -- the cartridge carries no mangled names at all -- and the
 * class-typed parameter is what cost +0x18, because the body reads `speed` in
 * both branches. Retail reads the argument straight out of r3 (`mov r4, r3`),
 * so the honest parameter is the scalar Fix12i and the honest startFrame is the
 * u16 the body already loads with `ldrh`. Under that spelling the method
 * reproduces 0x02016748/0x5c exactly. See notes/mwccarm-codegen.md section 6az,
 * MEASURED 2026-10-22.
 *
 * Two call sites could not follow: d_a_ots.cpp tail-forwards the result as an
 * int, and one Player.cpp body passes a non-constant startFrame. Each keeps a
 * scalar `local extern:` with its reason.
 */

#ifdef __cplusplus

namespace Memory {
void operator_delete2(void *);
}

struct dExtAnmModel_c : dExtSimpleModel_c, dExtFrameCtrl_c {
    BCA_File *file;            /* 0x60 */

    /* --- primary vtable order. Do not reorder. --- */
    virtual ~dExtAnmModel_c();                            /* slots 0 (D1), 1 (D0) */
    virtual void UpdateVerts();                           /* slot 3 */
    virtual void Virtual10(Matrix4x3 &mat);               /* slot 4 */
    virtual void Render(const Vector3 *scale);            /* slot 5 */
    virtual void Virtual18(u32 mat, const Vector3 *scale);/* slot 6 */

    /* DECLARED, defined out of line in src/engine/model/d_ext_model.cpp as real C++
     * -- complete-object context for every ROM caller (103 of them), hence C1.
     * Body is the single store file = 0; both base steps and both vptr stores
     * are synthesised (notes/ctor-migration.md section 6). The C2 sibling each
     * TU emits is stripped by objisolate; its enrolled home stays the
     * hand-written _ZN14dExtAnmModel_cC2Ev.c (section 1). */

    /* --- non-virtual --- */
    dExtAnmModel_c();
    void SetAnim(BCA_File *animFile, int flags, Fix12i speed, u16 startFrame);
    void Copy(const dExtAnmModel_c &src, char *newFile);

    /* ITS OWN, TO RESOLVE AN AMBIGUITY MULTIPLE INHERITANCE CREATES. dExtAnmModel_c
       derives from dExtSimpleModel_c (so dExtModel_c) and from dExtFrameCtrl_c, and both bases
       declare operator delete, so an inherited one is "ambiguous access to
       name found: dExtModel_c::operator delete and dExtFrameCtrl_c::operator delete".
       Declaring it here picks the same deallocator both bases name, and also
       satisfies the rule in include/dActor_c.h that mwcc only inlines the member
       when it is in the class or its immediate base. */
    void operator delete(void *ptr) {
        Memory::operator_delete2(ptr);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dExtAnmModel_c_size_must_be_0x64[sizeof(dExtAnmModel_c) == 0x64 ? 1 : -1];
#endif

#else

/* The same object for C translation units, both vptrs written out. */
struct dExtAnmModel_c {
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

/* So an object header declaring a dExtAnmModel_c member reads the same in both modes: C++
 * gets the class, C gets the flat stand-in above, and neither needs `struct`. */
typedef struct dExtAnmModel_c dExtAnmModel_c;

#endif /* __cplusplus */

#endif /* DEXTANMMODEL_C_H */
