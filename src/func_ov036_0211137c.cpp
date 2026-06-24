//cpp
typedef int Fix12;
struct BMD_File;
struct KCL_File;
struct SharedFilePtr;
struct Matrix4x3;
struct CLPS_Block;
struct Model { static BMD_File *LoadFile(SharedFilePtr &p); };
struct ModelBase { void SetFile(BMD_File *f, int a, int b); };
struct Platform { void UpdateClsnPosAndRot(); };
struct MeshCollider { static KCL_File *LoadFile(SharedFilePtr &p); };
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, Fix12 a, short b, CLPS_Block &cb);
};

extern SharedFilePtr data_ov036_02114028;
extern SharedFilePtr data_ov036_02114020;
extern CLPS_Block data_ov036_02112b68;
extern "C" void func_ov036_0211123c(char *t);
extern "C" void func_020393d4(int *p, int v);
extern "C" void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_();

extern "C" int func_ov036_0211137c(char *c)
{
    BMD_File *f = Model::LoadFile(data_ov036_02114028);
    ((ModelBase *)(c + 0xd4))->SetFile(f, 1, -1);
    *(short *)(c + 0x90) = 0x2000;
    func_ov036_0211123c(c);
    ((Platform *)c)->UpdateClsnPosAndRot();
    KCL_File *k = MeshCollider::LoadFile(data_ov036_02114020);
    ((MovingMeshCollider *)(c + 0x124))->SetFile(k, *(Matrix4x3 *)(c + 0x2ec), 0x1000, *(short *)(c + 0x8e), data_ov036_02112b68);
    func_020393d4((int *)(c + 0x124), (int)&_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
    return 1;
}
