//cpp
struct BMD_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr; struct Vector3_16;
extern "C" void *ModelLoadFile(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
struct Platform { void UpdateModelPosAndRotY(); void UpdateClsnPosAndRot(); };
extern "C" void func_ov002_020baf80(char *t);
extern "C" void *MeshColliderLoadFile(void *fp);
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
};
struct MovingCylinderClsn {
    void Init(Actor *a, int fix, int t, unsigned int u0, unsigned int u1);
};
struct WithMeshClsn {
    void Init(Actor *a, int fix, int t, Vector3_16 *v, int t2);
    void StartDetectingWater();
};
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[2];      // pad to 0x50 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};
extern "C" char data_ov002_0210e064;
extern "C" char data_ov002_0210e05c;
extern "C" CLPS_Block data_ov002_0210d714;

struct V3 { int x, y, z; };

extern "C" int _ZN8SignPost13InitResourcesEv(char *self)
{
    void *mf = ModelLoadFile(&data_ov002_0210e064);
    ((ModelBase*)(self + 0xd4))->SetFile((BMD_File*)mf, 1, -1);
    ((ShadowModel*)(self + 0x358))->InitCuboid();

    int py = *(int*)(self + 0x60);
    int pz = *(int*)(self + 0x64);
    int px = *(int*)(self + 0x5c);
    int py2 = py + 0x64000;
    V3 v = { px, py2, pz };
    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    if (rg.DetectClsn() != 0)
        *(int*)(self + 0x60) = rg.result;

    ((Platform*)self)->UpdateModelPosAndRotY();
    ((Platform*)self)->UpdateClsnPosAndRot();
    func_ov002_020baf80(self);

    void *kf = MeshColliderLoadFile(&data_ov002_0210e05c);
    ((MovingMeshCollider*)(self + 0x124))->SetFile((KCL_File*)kf,
        *(Matrix4x3*)(self + 0x2ec), 0x199, *(short*)(self + 0x8e), data_ov002_0210d714);

    ((MovingCylinderClsn*)(self + 0x320))->Init((Actor*)self, 0x64000, 0x64000, 0x4800002, 0x41000);

    *(char*)(self + 0x58e) = 2;
    *(int*)(self + 0x3b0) = *(int*)(self + 0x5c);
    *(int*)(self + 0x3b4) = *(int*)(self + 0x60);
    *(int*)(self + 0x3b8) = *(int*)(self + 0x64);
    *(short*)(self + 0x3bc) = *(short*)(self + 0x8c);
    *(short*)(self + 0x3be) = *(short*)(self + 0x8e);
    *(short*)(self + 0x3c0) = *(short*)(self + 0x90);
    *(int*)(self + 0x9c) = -0x2000;
    *(int*)(self + 0xa0) = -0x3c000;

    ((WithMeshClsn*)(self + 0x3c8))->Init((Actor*)self, 0x28000, 0x28000, (Vector3_16*)0, 0);
    ((WithMeshClsn*)(self + 0x3c8))->StartDetectingWater();

    return 1;
}
