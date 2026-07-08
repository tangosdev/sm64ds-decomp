//cpp
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
extern "C" void *ModelLoadFile(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
struct Platform { void UpdateModelPosAndRotY(); void UpdateClsnPosAndRot(); };
extern "C" void *MeshColliderLoadFile(void *fp);
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
};
extern "C" void func_020393d4(void *p, void *v);
extern "C" void func_020393a4(void *p, int v);
extern "C" void func_02039394(void *p, int v);
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};
extern "C" char data_ov065_0211d904;
extern "C" char data_ov065_0211d90c;
extern "C" char func_02112258;
extern "C" unsigned char LEVEL_SPECIFIC_SETTING;
extern "C" short data_ov065_0211c0c8;
extern "C" void UpdatePosWithTransformSym();

struct V3 { int x, y, z; };

extern "C" int func_ov065_0211b1d4(char *self)
{
    void *mf = ModelLoadFile(&data_ov065_0211d904);
    ((ModelBase*)(self + 0xd4))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)(self + 0x33c))->InitCuboid();
    ((Platform*)self)->UpdateModelPosAndRotY();
    ((Platform*)self)->UpdateClsnPosAndRot();
    void *kf = MeshColliderLoadFile(&data_ov065_0211d90c);
    ((MovingMeshCollider*)(self + 0x124))->SetFile((KCL_File*)kf,
        *(Matrix4x3*)(self + 0x2ec), 0x199, *(short*)(self + 0x8e), *(CLPS_Block*)&func_02112258);
    func_020393d4(self + 0x124, (void*)&UpdatePosWithTransformSym);
    func_020393a4(self + 0x124, 0x1c0000);
    func_02039394(self + 0x124, 0x1000);
    *(short*)(self + 0x94) = *(short*)(self + 0x8e);
    *(int*)(self + 0x320) = *(int*)(self + 0x5c);
    *(int*)(self + 0x324) = *(int*)(self + 0x60);
    *(int*)(self + 0x328) = *(int*)(self + 0x64);
    *(short*)(self + 0x334) = (&data_ov065_0211c0c8)[LEVEL_SPECIFIC_SETTING];

    V3 v;
    v.x = *(int*)(self + 0x5c);
    v.y = *(int*)(self + 0x60);
    v.z = *(int*)(self + 0x64);
    v.y = v.y - 0xa000;
    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    *(int*)(self + 0x338) = v.y;
    if (rg.DetectClsn() != 0)
        *(int*)(self + 0x338) = rg.result;
    return 1;
}
