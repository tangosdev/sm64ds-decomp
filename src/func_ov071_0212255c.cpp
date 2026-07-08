//cpp
typedef int Fix12;
typedef short s16;

struct Vector3 { int x, y, z; };
struct Matrix4x3;
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct CLPS_Block;

struct Model {
    static BMD_File* LoadFile(SharedFilePtr& f);
};
struct ModelBase {
    int SetFile(BMD_File* f, int a, int b);
};
struct MeshCollider {
    static KCL_File* LoadFile(SharedFilePtr& f);
};
struct MovingMeshCollider {
    int SetFile(KCL_File* f, const Matrix4x3& m, Fix12 s, short n, CLPS_Block& c);
};
struct Platform {
    void UpdateClsnPosAndRot();
};

BMD_File* Model::LoadFile(SharedFilePtr&);
int ModelBase::SetFile(BMD_File*, int, int);
KCL_File* MeshCollider::LoadFile(SharedFilePtr&);
int MovingMeshCollider::SetFile(KCL_File*, const Matrix4x3&, Fix12, short, CLPS_Block&);
void Platform::UpdateClsnPosAndRot();

extern "C" {
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern void Vec3_Add(Vector3* out, Vector3* a, Vector3* b);
extern void func_ov071_02122080(char* t);
extern void func_020393d4(int* p, int v);

extern SharedFilePtr data_ov071_021230d0;
extern char MATRIX_SCRATCH_PAPER;
extern SharedFilePtr data_ov071_021230d8;
extern CLPS_Block data_ov063_0211ebd8;
}

extern int _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

extern "C" int func_ov071_0212255c(char* self) {
    ((ModelBase*)(self + 0xd4))->SetFile(Model::LoadFile(data_ov071_021230d0), 1, -1);
    *(int*)(self + 0x9c) = -0x2000;
    *(int*)(self + 0xa0) = -0x3c000;
    Vector3 in;
    Vector3 out;
    in.x = 0;
    in.y = 0;
    in.z = 0xc8000;
    out.x = 0;
    out.y = 0;
    out.z = 0;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(self + 0x8e));
    MulVec3Mat4x3(&in, &MATRIX_SCRATCH_PAPER, &out);
    Vector3 res;
    Vec3_Add(&res, (Vector3*)(self + 0x5c), &out);
    *(int*)(self + 0x5c) = res.x;
    *(int*)(self + 0x60) = res.y;
    *(int*)(self + 0x64) = res.z;
    func_ov071_02122080(self);
    ((Platform*)self)->UpdateClsnPosAndRot();
    ((MovingMeshCollider*)(self + 0x124))->SetFile(
        MeshCollider::LoadFile(data_ov071_021230d8),
        *(Matrix4x3*)(self + 0x2ec), 0x199, *(s16*)(self + 0x8e), data_ov063_0211ebd8);
    func_020393d4((int*)(self + 0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
