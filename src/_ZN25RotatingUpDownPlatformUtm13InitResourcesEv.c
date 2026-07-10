enum Bool { FALSE, TRUE };

typedef struct { int x, y, z; } Vector3;
struct Matrix4x3 { int m[12]; };
struct RaycastGround { char buf[0x44]; int f44; char rest[8]; };

extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *self, int a, int b, int c, int d);
extern int _ZN11ShadowModel10InitCuboidEv(void *self);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *pos, void *rot, int e, int f);
extern void Matrix4x3_FromRotationY(struct Matrix4x3 *m, short angY);
extern void MulVec3Mat4x3(const Vector3 *v, const struct Matrix4x3 *m, Vector3 *out);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
extern void func_ov091_02131340(void *c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(void *p, void *v);
extern void _ZN13RaycastGroundC1Ev(struct RaycastGround *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(struct RaycastGround *self, const Vector3 *v, void *a);
extern int _ZN13RaycastGround10DetectClsnEv(struct RaycastGround *self);
extern void _ZN13RaycastGroundD1Ev(struct RaycastGround *self);

extern signed char data_0209f2f8;
extern struct Matrix4x3 data_020a0e68;
extern char data_ov091_02134cdc[];
extern char data_ov091_02134d1c[];
extern void *data_ov091_02134c30[];
extern void *data_ov091_02134c34[];
extern void *data_ov091_02134c38[];
extern void *_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

int _ZN25RotatingUpDownPlatformUtm13InitResourcesEv(char *c)
{
    Vector3 v;
    Vector3 rotated;
    Vector3 posVec;
    Vector3 v2;
    struct RaycastGround rg;
    unsigned char idx394, idx395;
    void *bmd;
    void *kcl;

    if (*(unsigned int*)(c + 8) == 0xffff) {
        int *p = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p & ~2;
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(c, 0, 0x1000, 0, 0);
        return 1;
    }

    *(int*)(c + 0x388) = *(int*)(c + 0x5c);
    *(int*)(c + 0x38c) = *(int*)(c + 0x60);
    *(int*)(c + 0x390) = *(int*)(c + 0x64);
    _ZN11ShadowModel10InitCuboidEv(c + 0x320);

    {
        enum Bool isSpecial;
        c[0x395] = 0;
        isSpecial = (enum Bool)(*(unsigned short*)(c + 0xc) == 0x1e);
        if (isSpecial) {
            c[0x395] = 2;
        } else if (data_0209f2f8 == 7) {
            c[0x395] = 1;
        }
    }

    c[0x394] = (unsigned char)(*(unsigned int*)(c + 8) & 0xf);

    if ((unsigned char)c[0x394] == 2) {
        char *tbl = data_ov091_02134cdc;
        int s = 0x78;
        int i = (unsigned char)c[0x395];
        Vec3_Add(&v, (Vector3*)(c + 0x388), (Vector3*)(tbl + i * s));
        i = (unsigned char)c[0x395];
        v.y += *(int*)(data_ov091_02134d1c + i * s);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x1d, 0xffff, &v, 0, *(signed char*)(c + 0xcc), -1);
    }

    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c + 0x8e));

    {
        char *tbl = data_ov091_02134cdc;
        int s = 0x78;
        int i = (unsigned char)c[0x395];
        idx394 = (unsigned char)c[0x394];
        MulVec3Mat4x3((Vector3*)(tbl + i * s + idx394 * 0xc), &data_020a0e68, &rotated);
    }

    Vec3_Add(&v2, (Vector3*)(c + 0x388), &rotated);

    *(int*)(c + 0x5c) = v2.x;
    *(int*)(c + 0x60) = v2.y;
    *(int*)(c + 0x64) = v2.z;

    idx395 = (unsigned char)c[0x395];
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov091_02134c30 + idx395 * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, -1);

    func_ov091_02131340(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    idx395 = (unsigned char)c[0x395];
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov091_02134c34 + idx395 * 0xc));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, kcl, c + 0x2ec, 0x199, *(short*)(c + 0x8e),
        *(void**)((char*)data_ov091_02134c38 + idx395 * 0xc));

    func_020393d4(c + 0x124, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    posVec.x = *(int*)(c + 0x5c);
    posVec.y = *(int*)(c + 0x60);
    posVec.z = *(int*)(c + 0x64);
    posVec.y -= 0x14000;

    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &posVec, 0);
    *(int*)(c + 0x37c) = posVec.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        *(int*)(c + 0x37c) = rg.f44;

    *(short*)(c + 0x380) = *(short*)(c + 0x8c);
    *(short*)(c + 0x382) = *(short*)(c + 0x8e);
    *(short*)(c + 0x384) = *(short*)(c + 0x90);

    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
