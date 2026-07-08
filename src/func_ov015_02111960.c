// NONMATCHING: different op / idiom (div=29). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef struct Vec3 { int x, y, z; } Vec3;

typedef struct RaycastGround {
    char pad[0x44];
    int clsnY;
    char pad2[0x8];
} RaycastGround;

extern int data_ov015_0211497c;
extern int data_ov015_02114974;
extern int data_ov015_02113574;
extern int MATRIX_SCRATCH_PAPER;
extern signed char LEVEL_ID;
extern unsigned char STAR_ID;

void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void* self);
void func_ov015_0211166c(char* t);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int fix, short s, void* clps);
void Matrix4x3_FromRotationY(void* m, int angle);
void MulVec3Mat4x3(Vec3* v, void* m, Vec3* out);
void Vec3_Add(Vec3* out, Vec3* a, Vec3* b);
void _ZN13RaycastGroundC1Ev(RaycastGround* self);
void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround* self, Vec3* v, void* a);
int _ZN13RaycastGround10DetectClsnEv(RaycastGround* self);
int IsStarCollectedInCurLevel(void);
void _ZN13RaycastGroundD1Ev(RaycastGround* self);

int func_ov015_02111960(char* self)
{
    RaycastGround rg;
    Vec3 a;
    Vec3 b;
    Vec3 c;
    Vec3 d;
    int dist;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov015_0211497c), 1, -1);
    _ZN11ShadowModel10InitCuboidEv(self + 0x320);
    func_ov015_0211166c(self);
    _ZN8Platform19UpdateClsnPosAndRotEv(self);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov015_02114974),
        self + 0x2ec, 0x1000, *(short*)(self + 0x8e), &data_ov015_02113574);
    *(short*)(((int)self + 0x8e) & 0xFFFFFFFFFFFFFFFF) += 0x8000;

    dist = -0xfa000;
    a.z = dist;
    a.x = 0;
    a.y = 0;
    b.x = 0;
    b.y = 0;
    b.z = 0;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(self + 0x8e));
    MulVec3Mat4x3(&a, &MATRIX_SCRATCH_PAPER, &b);
    Vec3_Add(&c, &b, (Vec3*)(self + 0x5c));
    c.y += 0x14000;
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &c, 0);
    *(int*)(self + 0x384) = c.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        *(int*)(self + 0x384) = rg.clsnY;

    dist = 0x32000;
    a.z = dist;
    a.x = 0;
    a.y = 0;
    b.x = 0;
    b.y = 0;
    b.z = 0;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(self + 0x8e));
    MulVec3Mat4x3(&a, &MATRIX_SCRATCH_PAPER, &b);
    Vec3_Add(&d, &b, (Vec3*)(self + 0x5c));
    *(int*)(self + 0x378) = d.x;
    *(int*)(self + 0x37c) = d.y;
    *(int*)(self + 0x380) = d.z;
    *(int*)(self + 0x388) = *(int*)(self + 0x60);
    *(unsigned char*)(self + 0x396) = 1;

    if (LEVEL_ID == 7 && (STAR_ID == 1 || !IsStarCollectedInCurLevel())
        && *(int*)(self + 0x60) >= 0xdac000)
    {
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
