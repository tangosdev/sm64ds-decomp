#ifndef MODELBASE_H
#define MODELBASE_H

#include "types.h"
#include "BMD_File.h"
#include "math/Matrix.h"

/* The root of the model hierarchy, vtable at 0x0208e87c (data_0208e87c).
 *
 * The chain is ModelBase -> Model -> (the ModelAnim family), with CommonModel
 * and ShadowModel as further ModelBase-derived siblings. Model derives from
 * ModelBase DIRECTLY: Model::C2 calls ModelBase::C1 and then stores its own
 * vptr, one intermediate store, no more.
 *
 * VTABLE is read out of the ROM at 0x0208e87c:
 *
 *   slot 0  0x02017120  ~ModelBase (D1)
 *   slot 1  0x020170e8  ~ModelBase (D0)
 *   slot 2  0x00000000  DoSetFile -- null, so it is pure virtual here
 *
 * Slot 2 being real is pinned by ModelBase::SetFile (0x02016fd4), which
 * dispatches vtable+0x8 on a ModelBase*; every derived vtable (CommonModel,
 * Model) carries its DoSetFile override in that slot.
 *
 * THE KEY-FUNCTION RULE, AND WHY IT NO LONGER FORBIDS A REAL D1. The
 * destructor is declared first, which makes it the key function, and CW 1.2
 * emits the class vtable into whichever TU defines the key function --
 * colliding with the copy the module's gap object already supplies from ROM
 * data. That is why the D0/D1/D2 bodies in this family were all C translation
 * units that never saw the class: no TU defined the key function, so no vtable
 * was ever emitted. Same arrangement as ActorBase::InitResources, see
 * include/ActorBase.h and PR #974.
 *
 * tools/objisolate.py retired that constraint. It keeps the declared
 * function's .text, drops the vtable and typeinfo the TU emitted alongside it,
 * and rebinds the reference to the ROM's own carved-out _ZTV symbol, so a
 * key-function TU is eligible after all. The rule was real and not imagined:
 * run `python tools/eligible.py --no-isolate` and every D1 in this family that
 * has since become a real method drops straight back out of the eligible set.
 *
 * So D1 may be a real method wherever the layout is known. D2 stays a C file,
 * and D0 stays one for a different and permanent reason -- it is the deleting
 * destructor, which the compiler generates and nobody can write by hand, so
 * renaming it .cpp would change the extension without migrating anything.
 * ModelBase's own three are still C files; the derived classes have moved.
 *
 * What has NOT changed is the declaration order. The destructor stays first,
 * because for a root class vtable slot order IS declaration order.
 *
 * LAYOUT evidence: ModelBase::C1 stores the vptr at +0x0 and zeroes +0x4;
 * the destructors Deallocate +0x4 when set; Model::LoadAndSetFile stores the
 * loaded file at +0x4. The base ENDS at 0x8. Each derived class puts its
 * view of the components at +0x8 -- Model EMBEDS a ModelComponents there,
 * while CommonModel and ShadowModel store a ModelComponents POINTER (their
 * constructors then place their own fields from +0xc, inside what an
 * embedded struct would occupy, which is what rules the embed out of the
 * base). ApplyOpacity still addresses +0x8 blindly; see the note in its
 * source file.
 */

#ifdef __cplusplus

struct BCA_File;

/* 0x14 bytes, at +0x8 of a Model (embedded) or behind +0x8 of a CommonModel
   or ShadowModel (pointer to a pool entry from func_02016e70). The runtime
   view of a loaded model: built from the BMD file by func_020462d0,
   rendered by 0x020443c8. */
struct ModelComponents {
    BMD_File *modelFile;        /* 0x00 */
    BMD_Material *materials;    /* 0x04 - 0x30-byte records, flags at +0x24 */
    BMD_Bone *bones;            /* 0x08 */
    Matrix4x3 *transforms;      /* 0x0c - one per bone */
    void *unk_10;               /* 0x10 */

    void UpdateVertsUsingBones();               /* 0x02044dcc */
    void UpdateBones(BCA_File *file, int frame);
    void Render(Matrix4x3 *mat, Vector3 *scale); /* 0x020443c8 */
};

extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct ModelBase {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    BMD_File *modelFile;    /* 0x04 - owned; the destructors Deallocate it */

    /* --- vtable, in ROM order at 0x0208e87c. Do not reorder. --- */
    virtual ~ModelBase();                            /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b) = 0;  /* slot 2, null here */

    /* --- non-virtual --- */
    /* RETURNS int, not void. The definition at 0x02016fd4 is eight
       instructions: load the vtable, `blx` slot 2 (DoSetFile, which returns
       int), then the epilogue -- r0 is never touched after the call, so it
       flows straight out. Callers corroborate: KoopaShell::InitResources tests
       the result and bails on 0. */
    int SetFile(BMD_File *file, int a, int b);       /* dispatches DoSetFile */
    void ApplyOpacity(u32 a);

    /* WHAT LETS A REAL `~Class()` REPRODUCE THE ROM'S DELETING DESTRUCTOR.
       The compiler generates D0 as "run the destructor body, then call operator
       delete on the class". Without this it emits the global `_ZdlPv`, which
       exists nowhere in this image, and the D0 comes out one relocated word
       different from the ROM -- a difference build_pin.verify CANNOT SEE,
       because it wildcards relocated words. Only the link catches it.

       This family deallocates through Memory::operator_delete2, not the actor
       heap: every D0 below ends with a call to 0x0203cbcc. Actor's copy of this
       member calls Memory::Deallocate instead, which is why each needs its own.

       Inline, and in the IMMEDIATE base -- mwcc inlines it only when it finds it
       in the class or one level up, as include/Actor.h records. No layout
       effect: a non-virtual inline member adds no field and no vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

};

typedef char ModelComponents_size_must_be_0x14[sizeof(ModelComponents) == 0x14 ? 1 : -1];
typedef char ModelBase_size_must_be_0x8[sizeof(ModelBase) == 0x8 ? 1 : -1];

#else

/* The same objects for C translation units, vptr written out explicitly. */
struct ModelComponents {
    struct BMD_File *modelFile;        /* 0x00 */
    struct BMD_Material *materials;    /* 0x04 */
    struct BMD_Bone *bones;            /* 0x08 */
    struct Matrix4x3 *transforms;      /* 0x0c */
    void *unk_10;                      /* 0x10 */
};

struct ModelBase {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
};

#endif /* __cplusplus */

#endif /* MODELBASE_H */
