//cpp
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
extern "C" void *ModelLoadFile(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
extern "C" void func_ov035_021118a8(char *t);
struct Platform { void UpdateClsnPosAndRot(); };
extern "C" void *MeshColliderLoadFile(void *fp);
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
};
extern "C" void func_020393d4(void *p, void *v);
extern "C" void func_020396c0(void *p, int v);
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};
extern "C" char data_ov035_02112cb0;
extern "C" char data_ov035_02112cb8;
extern "C" CLPS_Block data_ov035_02112238;
extern "C" void UpdatePosWithTransformSym();

struct V3 { int x, y, z; };

extern "C" int func_ov035_02111a98(char *self)
{
    void *mf = ModelLoadFile(&data_ov035_02112cb0);
    ((ModelBase*)(self + 0xd4))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)(self + 0x328))->InitCuboid();
    func_ov035_021118a8(self);
    ((Platform*)self)->UpdateClsnPosAndRot();
    void *kf = MeshColliderLoadFile(&data_ov035_02112cb8);
    ((MovingMeshCollider*)(self + 0x124))->SetFile((KCL_File*)kf,
        *(Matrix4x3*)(self + 0x2ec), 0x1000, *(short*)(self + 0x8e), data_ov035_02112238);
    func_020393d4(self + 0x124, (void*)&UpdatePosWithTransformSym);
    func_020396c0(self + 0x124, 0);
    V3 v;
    v.x = *(int*)(self + 0x5c);
    v.y = *(int*)(self + 0x60);
    v.z = *(int*)(self + 0x64);
    v.y = v.y - 0xa000;
    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    *(int*)(self + 0x324) = v.y;
    if (rg.DetectClsn() != 0)
        *(int*)(self + 0x324) = rg.result;
    return 1;
}
