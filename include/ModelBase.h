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
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD. It is the
 * first virtual, which makes it the key function, and CW 1.2 emits the class
 * vtable into whichever TU defines the key function -- colliding with the
 * copy the module's gap object already supplies from ROM data. The D0/D1/D2
 * bodies stay C translation units (src/_ZN9ModelBaseD*Ev.c) that never see
 * this class, so no TU ever defines the key function and no vtable is ever
 * emitted. Same arrangement as ActorBase::InitResources, see
 * include/ActorBase.h and PR #974.
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

struct ModelBase {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    BMD_File *modelFile;    /* 0x04 - owned; the destructors Deallocate it */

    /* --- vtable, in ROM order at 0x0208e87c. Do not reorder. --- */
    /* THE DESTRUCTOR PAIR, SPELLED AS TWO PLAIN VIRTUALS ON THE HOST. mwccarm
       gives `virtual ~ModelBase()` TWO vtable entries -- the Itanium D1
       complete / D0 deleting pair this header's own map reads out of the ROM
       at slots 0 and 1 -- and MSVC folds them into ONE. Spelt as a destructor
       the declaration indexes correctly on the ARM and ONE SLOT EARLY on the
       host, for every virtual declared after it and all the way down the
       hierarchy: ModelBase -> Model -> ModelAnim -> {ModelAnim2,
       BlendModelAnim}, plus the CommonModel and ShadowModel siblings.

       That skew is what put ModelAnim::Render's ROM slot 5 on the host's
       Virtual18, the collision port/unmatched/ModelAnim_Renders.cpp,
       BobEnemy_Renders.cpp and W19_Slot5_Renders.cpp were written around --
       ROM-faithful Render bodies kept out of the link and hand-transcribed
       instead. MEASURED, not reasoned: the ROM's _ZTV9ModelAnim at 0x0208e980
       reads [0] D1, [1] D0, [2] Model::DoSetFile, [3] UpdateVerts,
       [4] Virtual10, [5] Render, [6] Virtual18, while MSVC's own
       /d1reportSingleClassLayoutModelAnim dropped D0 and read [1] DoSetFile
       ... [4] Render, [5] Virtual18 -- one slot early from index 1 on, and the
       missing word is exactly D0.

       Two ordinary virtuals occupy the same two entries under MSVC that the
       destructor pair occupies under mwccarm, so spelling them out on the host
       makes MSVC's numbering the ROM's numbering, and the port's tables are
       filled in ROM order throughout. The guard keeps the ARM side untouched:
       nothing in this tree defines _MSC_VER, so mwccarm still sees the
       destructor and no ROM byte moves. Neither name is ever called; they hold
       the two slots the ROM's table holds. Same shape as the CylinderClsn fix
       in include/CylinderClsn.h and the FaderBrightness fix in
       src/ProcessKuppaScript.cpp. */
#ifdef _MSC_VER
    virtual void Destructor1();                      /* slot 0 (D1) */
    virtual void Destructor0();                      /* slot 1 (D0) */
#else
    virtual ~ModelBase();                            /* slots 0 (D1), 1 (D0) */
#endif
    virtual int DoSetFile(char *file, int a, int b) = 0;  /* slot 2, null here */

    /* --- non-virtual --- */
    void SetFile(BMD_File *file, int a, int b);      /* dispatches DoSetFile */
    void ApplyOpacity(u32 a);
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
