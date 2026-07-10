//cpp
// NONMATCHING: missing logic (ROM does more) (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct BMD_File; struct BTA_File; struct KCL_File; struct Actor; struct Vector3; struct Matrix4x3;
struct CLPS_Block; struct SharedFilePtr;
extern "C" void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
struct ModelBase { void SetFile(BMD_File *f, int b, int c); };
struct ShadowModel { void InitCuboid(); };
struct TextureTransformer {
    void Prepare(BMD_File &f, BTA_File &b);
    void SetFile(BTA_File &f, int a, int fix, unsigned int u);
};
struct Platform {
    void UpdateModelPosAndRotY();
    void UpdateClsnPosAndRot();
};
extern "C" void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
struct MovingMeshCollider {
    void SetFile(KCL_File *f, const Matrix4x3 &m, int fix, short sh, CLPS_Block &b);
};
extern "C" void func_020393c4(void *p, void *v);
struct RaycastGround {
    int pad[0x11];
    int result;       // offset 0x44
    int pad2[3];      // pad to 0x54 total
    RaycastGround();
    void SetObjAndPos(const Vector3 &v, Actor *a);
    int DetectClsn();
    ~RaycastGround();
};

struct Entry3 { void *a; void *b; void *c; };

extern "C" char data_ov065_0211d16c[8];
extern "C" Entry3 data_ov065_0211d194[];
extern "C" Entry3 data_ov065_0211d198[];
extern "C" Entry3 data_ov065_0211d19c[];
extern "C" void func_ov065_0211aacc();
extern "C" unsigned char data_0209f2c0;
extern "C" void *data_ov065_0211c0b8[];

struct V3 { int x, y, z; };

extern "C" int _ZN20TtcConveyorBeltLarge13InitResourcesEv(char *self)
{
    int locbuf[2];
    locbuf[0] = *(int*)&data_ov065_0211d16c[0];
    locbuf[1] = *(int*)&data_ov065_0211d16c[4];

    if (*(unsigned short*)(self + 0xc) == 0x6f)
        goto notspecial;
    if (*(unsigned short*)(self + 0xc) == 0x70)
    {
        *(unsigned char*)(self + 0x39e) = 1;
        goto skip;
    }
notspecial:
    *(unsigned char*)(self + 0x39e) = 0;
skip:;

    unsigned char e = *(unsigned char*)(self + 0x39e);
    void *mf = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d194[e].a);
    ((ModelBase*)(self + 0xd4))->SetFile((BMD_File*)mf, 1, -1);

    ((ShadowModel*)(self + 0x334))->InitCuboid();

    e = *(unsigned char*)(self + 0x39e);
    ((TextureTransformer*)(self + 0x320))->Prepare(
        *(BMD_File*)*(void**)((char*)data_ov065_0211d194[e].a + 4),
        *(BTA_File*)&locbuf[e]);

    e = *(unsigned char*)(self + 0x39e);
    ((TextureTransformer*)(self + 0x320))->SetFile(*(BTA_File*)&locbuf[e], 0, 0x1000, 0);

    ((Platform*)self)->UpdateModelPosAndRotY();
    ((Platform*)self)->UpdateClsnPosAndRot();

    e = *(unsigned char*)(self + 0x39e);
    void *kf = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov065_0211d198[e].a);
    ((MovingMeshCollider*)(self + 0x124))->SetFile((KCL_File*)kf,
        *(Matrix4x3*)(self + 0x2ec), 0x199, *(short*)(self + 0x8e),
        *(CLPS_Block*)data_ov065_0211d19c[e].a);

    func_020393c4(self + 0x124, (void*)&func_ov065_0211aacc);

    *(void**)(self + 0x390) = data_ov065_0211c0b8[data_0209f2c0];
    *(void**)(self + 0x38c) = *(void**)(self + 0x390);
    *(void**)(self + 0x32c) = *(void**)(self + 0x38c);

    V3 v;
    v.x = *(int*)(self + 0x5c);
    v.y = *(int*)(self + 0x60);
    v.z = *(int*)(self + 0x64);
    v.y = v.y - 0xa000;

    RaycastGround rg;
    rg.SetObjAndPos(*(Vector3*)&v, (Actor*)0);
    *(int*)(self + 0x394) = v.y;
    if (rg.DetectClsn() != 0)
        *(int*)(self + 0x394) = rg.result;

    return 1;
}
