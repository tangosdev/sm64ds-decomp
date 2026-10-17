//cpp
/* Common-model renderer (arm9 .text 0x0201609c..0x02016254): dExtCommonModel_c, the
 * ModelBase sibling whose rendering state is a POINTER into a pooled
 * ModelComponents block (func_02016e70) rather than an in-place struct --
 * that is the whole difference from Model. DoSetFile takes its components
 * from the pool, ORs material flags through Func_020160AC, and tags the
 * model's polygon ID through SetPolygonID.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending.
 *
 * Vtable _ZTV17dExtCommonModel_c at 0x0208e8a4: [0] D1 0x020161e0, [1] D0
 * 0x020161b4, [2] DoSetFile 0x02016144. The cartridge does carry _ZTI17dExtCommonModel_c/_ZTS17dExtCommonModel_c
 * (0x0208e7a0/0x0208e8b0), but no data section is enrolled for this TU to
 * own them, so this TU compiles with RTTI off (see below), and no enrolled D2 or C2;
 * those variants are licensed compiler-only in the manifest.
 *
 * leftovers:
 * - common.h must come first: the ctor's `mat4x3 = IDENTITY_MATRIX4X3`
 *   needs the flat Matrix4x3 { s32 m[12]; } spelling for its block copy;
 *   math/Matrix.h's nested form scalarizes it.
 * - RTTI stays off: the cartridge's _ZTI/_ZTS records sit in data this TU
 *   does not own.
 * - func_02016e70, func_02046008, MulMat4x3Mat4x3, data_0209b3ec and
 *   IDENTITY_MATRIX4X3 have no declaring header.
 * - The (u32 *)(&p->flags) launder in Func_020160AC keeps the ROM's
 *   materialized address for the read-modify-write.
 */

/* common.h comes first so this TU sees the flat Matrix4x3 { s32 m[12]; }
 * spelling: the ctor's mat4x3 = IDENTITY_MATRIX4X3 copy block-moves twelve
 * words under it, where the structured spelling scalarizes and lands +0x1c
 * bytes long. Same recipe as Model's constructor. */
#include "common.h"
#include "dExtCommonModel_c.h"

/* The cartridge's _ZTI/_ZTS records for dExtCommonModel_c (0x0208e7a0/0x0208e8b0)
 * sit in data this TU does not own, so records emitted here would have no
 * configured home. Compiling with RTTI off emits none; the vtable preamble's
 * typeinfo word deadstrips with the rest of the data sections. */
#pragma RTTI off

extern "C" {
extern Matrix4x3 data_0209b3ec;      /* the camera-space matrix Model::Render uses */
ModelComponents *func_02016e70(BMD_File *file);
void func_02046008(ModelComponents *data, u32 id);
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
}
extern Matrix4x3 IDENTITY_MATRIX4X3;

// @symbol _ZN17dExtCommonModel_cC1Ev
dExtCommonModel_c::dExtCommonModel_c() : data(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}

// @symbol _ZN17dExtCommonModel_cD1Ev
/* An empty body; the vtable store at the top and the ModelBase subobject
 * call at the bottom are what `struct dExtCommonModel_c : ModelBase` and
 * `virtual ~dExtCommonModel_c()` already mean, and the compiler emits them around
 * it. Neither member has a destructor, so the ROM's 0x24 bytes are exactly
 * that. D0 is generated from the same body plus the class operator delete. */
dExtCommonModel_c::~dExtCommonModel_c()
{
}

// @symbol _ZN17dExtCommonModel_c9DoSetFileEPcii
int dExtCommonModel_c::DoSetFile(char *file, int a, int b)
{
    data = func_02016e70((BMD_File *)file);
    if (data == 0)
        return 0;
    data->UpdateVertsUsingBones();
    if (a != 0)
        Func_020160AC(0x8000);
    if (b < 0)
        return 1;
    SetPolygonID((u32)b & 0xff);
    return 1;
}

// @symbol _ZN17dExtCommonModel_c6RenderEPK7Vector3
void dExtCommonModel_c::Render(const Vector3 *scale)
{
    Matrix4x3 temp;
    MulMat4x3Mat4x3(&mat4x3, &data_0209b3ec, &temp);
    data->Render(&temp, (Vector3 *)scale);
}

// @symbol _ZN17dExtCommonModel_c13Func_020160ACEj
/* ORs flags into every material; the (u32 *) launder on &p->flags keeps the
   ROM's materialized address for the read-modify-write. */
void dExtCommonModel_c::Func_020160AC(u32 flags)
{
    BMD_File *file = data->modelFile;
    u32 n = file->numMaterials;
    BMD_Material *p = data->materials;
    u32 i;

    for (i = 0; i < n; i++) {
        u32 *f = (u32 *)(&p->flags);
        *f |= flags;
        p++;
    }
}

// @symbol _ZN17dExtCommonModel_c12SetPolygonIDEj
void dExtCommonModel_c::SetPolygonID(u32 id)
{
    func_02046008(data, id);
}
