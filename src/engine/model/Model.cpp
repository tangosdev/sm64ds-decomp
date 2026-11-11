//cpp
/* Model -- the concrete model class; the cartridge calls it dExtSimpleModel_c
   (_ZTI17dExtSimpleModel_c at 0x0208e794, _ZTV5Model at 0x0208e90c). TU claims
   0x02016b78..0x02016df8, the whole class run in delinks order: Render,
   Virtual10, DoSetFile, UpdateVerts, then the full lifecycle D2,D0,D1,C1,C2
   -- every emitted variant has a cartridge home, so nothing deadstrips.
   Written back-to-front: mwccarm emits .text in reverse source order under
   default deferred codegen. */

/* common.h comes first so this TU sees the flat Matrix4x3 { s32 m[12]; }
   spelling: the ctor's mat4x3 = IDENTITY_MATRIX4X3 copy block-moves twelve
   words under it, where the structured Matrix3x3+Vector3 spelling
   scalarizes and lands +0x1c bytes long. */
#include "common.h"
#include "Model.h"

/* The cartridge homes this class's RTTI under the dExtSimpleModel_c spelling, so
   _ZTS5Model/_ZTI5Model records emitted under the project spelling would
   have no configured home. Compiling with RTTI off emits no records at
   all; the vtable preamble's typeinfo word deadstrips with the rest of the
   data sections. */
#pragma RTTI off

extern Matrix4x3 IDENTITY_MATRIX4X3;
extern "C" {
extern Matrix4x3 data_0209b3ec;      /* local extern: the camera-space matrix Render uses */
/* local extern: no header declares it */
void MulMat4x3Mat4x3(const Matrix4x3 *m1, const Matrix4x3 *m0, Matrix4x3 *mF);
/* local extern: no header declares it; data.transforms-bone walk in Virtual10 */
void func_02045074(ModelComponents *data, BMD_Bone *bones);
/* local extern: no header declares it; sizes the transforms buffer from the file */
u32 func_02046564(BMD_File *file);
/* local extern: decl_common.h's copy is the same mangled spelling; no
   namespace-Memory header declares operator_new2 */
void *_ZN6Memory13operator_new2Ej(u32 size);
/* local extern: no header declares it; initializes ModelComponents over the file */
void func_020462d0(ModelComponents *data, BMD_File *file, void *buffer);
/* local extern: no header declares it; neighbour below this run */
void func_02016b24(Model *self, int flags);
/* local extern: no header declares it; delete veneer for the transforms buffer */
void func_0203cbc0(void *ptr);
}

/* One definition emits C1 and C2: the base-chain call to ModelBase::C2, the
   _ZTV5Model vptr store, the flat twelve-word identity copy, and the
   transformsBuf zero. The cartridge keeps both variants. */
// @symbol _ZN5ModelC2Ev
// @symbol _ZN5ModelC1Ev
Model::Model() : transformsBuf(0)
{
    mat4x3 = IDENTITY_MATRIX4X3;
}

/* One definition emits D2, D0 and D1: the vptr store and the ModelBase
   destructor call are the compiler's; the body is the class's one written
   obligation -- handing transformsBuf back through the delete veneer. D0
   follows with the ModelBase operator delete (Memory::operator_delete2). */
// @symbol _ZN5ModelD1Ev
Model::~Model()
{
    if (transformsBuf)
        func_0203cbc0(transformsBuf);
}

// @symbol _ZN5Model11UpdateVertsEv
void Model::UpdateVerts()
{
    data.UpdateVertsUsingBones();
}

// @symbol _ZN5Model9DoSetFileEPcii
int Model::DoSetFile(char *file_, int a, int b)
{
    BMD_File *file = (BMD_File *)file_;
    void *buffer;
    AddToCommonModelDataArr(*file);
    transformsBuf = _ZN6Memory13operator_new2Ej(func_02046564(file));
    buffer = transformsBuf;
    if (buffer == 0) return 0;
    func_020462d0(&data, file, buffer);
    data.UpdateVertsUsingBones();
    if (a != 0) func_02016b24(this, 0x8000);
    if (b < 0) return 1;
    SetPolygonID(b & 0xff);
    return 1;
}

// @symbol _ZN5Model9Virtual10ER9Matrix4x3
void Model::Virtual10(Matrix4x3 &mat)
{
    /* The copy is one flat 12-word ldm/stm run in the ROM. CW copies the
       nested Matrix4x3 (Matrix3x3 + Vector3) memberwise, which is bigger,
       so it goes through a flat 12-word view instead. */
    struct Mtx12w { Fix12i m[12]; };
    *(Mtx12w *)data.transforms = *(const Mtx12w *)&mat;
    func_02045074(&data, data.modelFile->bones);
}

#ifdef _MSC_VER
extern "C" int host_widen_model_skip(const void *mat4x3);   /* ORIGINOBJ1, src/runtime/actor/d_actor.cpp: a never-posed far model */
#endif
// @symbol _ZN5Model6RenderEPK7Vector3
void Model::Render(const Vector3 *scale)
{
    Matrix4x3 temp;
#ifdef _MSC_VER
    if (host_widen_model_skip(&mat4x3))
        return;
#endif
    MulMat4x3Mat4x3(&mat4x3, &data_0209b3ec, &temp);
    data.Render(&temp, (Vector3 *)scale);
}
