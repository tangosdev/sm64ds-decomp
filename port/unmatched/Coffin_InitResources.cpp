/* HOST MIRROR of the matched src/_ZN6Coffin13InitResourcesEv.cpp with
 * exactly FIVE LINES DELETED: the file-scope out-of-class member
 * REDECLARATIONS (`BMD_File* Model::LoadFile(SharedFilePtr&);` and four
 * siblings). mwcc accepts a redundant namespace-scope redeclaration of a
 * member; MSVC rejects it outright (C2761), so the matched text cannot
 * compile on the host. Nothing else differs -- diff against the src TU to
 * verify. Three of the five methods carry return-type spellings no linked
 * TU emits (int SetFile vs the linked void SetFile, typed LoadFile vs the
 * linked char* one); those bridge through the faces at the bottom of
 * port/unmatched/Bbh_MethodFaces.cpp.
 */
//cpp
// @symbol _ZN6Coffin13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Coffin.h"
typedef int CoffinShimFix12;   /* was `Fix12`: main's math/Fix12.h now owns
                                  that name (C2921). Still int, so the shim
                                  method below decorates unchanged. */
typedef short s16;

struct Matrix4x3;
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct CLPS_Block;

/* main -> port sync (SYNC4): include/Coffin.h now reaches the real Model and
   ModelBase, which declare LoadFile and SetFile with these same signatures, so
   the two shims here became C2011 redefinitions. The call sites below are
   unchanged and decorate the same way. */
#include "Model.h"
#include "ModelBase.h"
struct MeshCollider {
    static KCL_File* LoadFile(SharedFilePtr& f);
};
struct MovingMeshCollider {
    int SetFile(KCL_File* f, const Matrix4x3& m, CoffinShimFix12 s, short n, CLPS_Block& c);
};
struct Platform {
    void UpdateClsnPosAndRot();
};


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

/* C LINKAGE, run link100 lane HOSTGEN4. This is the ADDRESS of a ROM body,
   taken as a datum because func_020393d4 is handed it and the mesh collider
   calls it later. Declared plain, MSVC emits the decorated
   ?_ZN4dBgW22UpdatePosWithTransform...@@3HA, one of four decorated spellings
   of the one ROM symbol that the fourteen translation units doing this
   produce between them. Under C linkage they collapse onto the flat name the
   ROM itself carries, which port/faces_sync.txt already has on the wall as an
   ordinary face row. The thirteen src/ copies get the same treatment through
   hostgen's DATA_C_LINKAGE table; this one is a host copy, so it is spelled
   here. */
extern "C" {
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
}

int Coffin::InitResources()
{
    ((ModelBase*)((char*)&mModel))->SetFile(
        (BMD_File*)Model::LoadFile(data_ov071_021230d0), 1, -1);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
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
    ((Platform*)((char*)this))->UpdateClsnPosAndRot();
    ((MovingMeshCollider*)((char*)&mMeshCollider))->SetFile(
        MeshCollider::LoadFile(data_ov071_021230d8),
        *(Matrix4x3*)((char*)&mClsnMat), 0x199, mAngleY, data_ov063_0211ebd8);
    func_020393d4((int*)((char*)&mMeshCollider), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    return 1;
}
