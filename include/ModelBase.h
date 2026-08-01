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
 * loaded file at +0x4; ModelBase::ApplyOpacity and Model::SetPolygonID both
 * hand +0x8 to ModelComponents functions.
 */

#ifdef __cplusplus

/* 0x14 bytes at +0x8 of every ModelBase. The runtime view of a loaded model:
   built from the BMD file by func_020462d0, rendered by 0x020443c8. */
struct ModelComponents {
    BMD_File *modelFile;        /* 0x00 */
    BMD_Material *materials;    /* 0x04 - 0x30-byte records, flags at +0x24 */
    BMD_Bone *bones;            /* 0x08 */
    Matrix4x3 *transforms;      /* 0x0c - one per bone */
    void *unk_10;               /* 0x10 */

    void UpdateVertsUsingBones();               /* 0x02044dcc */
    void Render(Matrix4x3 *mat, Vector3 *scale); /* 0x020443c8 */
};

struct ModelBase {
    /* 0x00 is the vptr, placed implicitly by the first virtual declaration. */
    BMD_File *modelFile;    /* 0x04 - owned; the destructors Deallocate it */
    ModelComponents data;   /* 0x08 */

    /* --- vtable, in ROM order at 0x0208e87c. Do not reorder. --- */
    virtual ~ModelBase();                            /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b) = 0;  /* slot 2, null here */

    /* --- non-virtual --- */
    void SetFile(BMD_File *file, int a, int b);      /* dispatches DoSetFile */
    void ApplyOpacity(u32 a);
};

typedef char ModelComponents_size_must_be_0x14[sizeof(ModelComponents) == 0x14 ? 1 : -1];
typedef char ModelBase_size_must_be_0x1c[sizeof(ModelBase) == 0x1c ? 1 : -1];

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
    struct ModelComponents data;       /* 0x08 */
};

#endif /* __cplusplus */

#endif /* MODELBASE_H */
