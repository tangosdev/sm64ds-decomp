//cpp
// @symbol _ZN6Coffin13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Coffin.h"
typedef int Fix12;

struct Matrix4x3;
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct CLPS_Block;

struct dBgW_Kc {
    static KCL_File* LoadFile(SharedFilePtr& f);
};
struct dBgW_KcMbg {
    int SetFile(KCL_File* f, const Matrix4x3& m, Fix12 s, short n, CLPS_Block& c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, KCL_File* f, const Matrix4x3& m, Fix12 s, short n, CLPS_Block& c);

struct dBgActor_c {
    void UpdateClsnPosAndRot();
};

KCL_File* dBgW_Kc::LoadFile(SharedFilePtr&);
int dBgW_KcMbg::SetFile(KCL_File*, const Matrix4x3&, Fix12, short, CLPS_Block&);
void dBgActor_c::UpdateClsnPosAndRot();

extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
extern void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov071_021230d0;
extern char data_020a0e68;
extern SharedFilePtr data_ov071_021230d8;
extern CLPS_Block data_ov063_0211ebd8;
}

extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

int Coffin::InitResources()
{
    ((ModelBase*)((char*)&mModel))->SetFile((BMD_File*)Model::LoadFile(data_ov071_021230d0), 1, -1);
    unk_09c = -0x2000;
    unk_0a0 = -0x3c000;
    Vector3 in;
    Vector3 out;
    in.x = 0;
    in.y = 0;
    in.z = 0xc8000;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&in, &data_020a0e68, &out);
    Vector3 res;
    Vec3_Add(&res, (Vector3*)((char*)&mPosX), &out);
    mPosX = res.x;
    mPosY = res.y;
    mPosZ = res.z;
    func_ov071_02122080(((char*)this));
    ((dBgActor_c*)((char*)this))->UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block((dBgW_KcMbg*)((char*)&mMeshCollider), dBgW_Kc::LoadFile(data_ov071_021230d8), *(Matrix4x3*)((char*)&unk_2ec), 0x199, mAngleY, data_ov063_0211ebd8);
    func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    return 1;
}
