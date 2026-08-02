#ifndef COMMONMODEL_H
#define COMMONMODEL_H

#include "types.h"
#include "BMD_File.h"
#include "ModelBase.h"
#include "math/Matrix.h"

/* ModelBase sibling with a POINTER to pooled components, vtable
 * _ZTV11CommonModel at 0x0208e8a4:
 *
 *   slot 0  0x020161e0  ~CommonModel (D1)
 *   slot 1  0x020161b4  ~CommonModel (D0)
 *   slot 2  0x02016144  DoSetFile(char *, int, int)
 *
 * Three slots, so unlike Model its Render is NON-virtual. DoSetFile takes
 * its components from the shared pool via func_02016e70 instead of building
 * them in place, which is the whole difference from Model.
 *
 * THE DESTRUCTOR IS DECLARED FIRST AND NEVER DEFINED AS A METHOD -- the
 * key-function arrangement from include/ModelBase.h. D0/D1 stay C files.
 *
 * LAYOUT evidence: C1 calls ModelBase::C1, stores the vptr, zeroes the
 * data pointer at +0x8 and copies mat4x3 from data_02082128 to +0xc. The
 * mat4x3 landing at +0xc is what pins the components as a pointer here and
 * rules the embedded struct out of ModelBase.
 *
 * Func_0201609C and Func_020160AC keep their placeholder names for now, but
 * the bodies are Model::SetPolygonID and the material-flag OR pass: both
 * are called from DoSetFile at exactly the positions Model::DoSetFile calls
 * its counterparts. Renaming them is a symbols.txt change and wants its own
 * PR with the S0_ lesson applied.
 */

#ifdef __cplusplus

struct CommonModel : ModelBase {
    ModelComponents *data;     /* 0x08 - pool entry from func_02016e70 */
    Matrix4x3 mat4x3;          /* 0x0c */

    /* --- vtable, in ROM order. Do not reorder. --- */
    virtual ~CommonModel();                           /* slots 0 (D1), 1 (D0) */
    virtual int DoSetFile(char *file, int a, int b);  /* slot 2 */

    /* --- non-virtual --- */
    void Render(const Vector3 *scale);
    void Func_0201609C(u32 id);      /* SetPolygonID by body */
    void Func_020160AC(u32 flags);   /* ORs flags into every material */
};

typedef char CommonModel_size_must_be_0x3c[sizeof(CommonModel) == 0x3c ? 1 : -1];

#else

struct CommonModel {
    void **vtable;                     /* 0x00 */
    struct BMD_File *modelFile;        /* 0x04 */
    struct ModelComponents *data;      /* 0x08 */
    struct Matrix4x3 mat4x3;           /* 0x0c */
};

#endif /* __cplusplus */

#endif /* COMMONMODEL_H */
