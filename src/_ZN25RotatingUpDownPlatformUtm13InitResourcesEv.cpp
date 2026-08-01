//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "RotatingUpDownPlatformUtm.h"
enum Bool { FALSE, TRUE };

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
extern void *data_ov091_02134c30[];
extern void *data_ov091_02134c34[];
extern void *_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

int RotatingUpDownPlatformUtm::InitResources()
{
    Vector3 v;
    Vector3 rotated;
    Vector3 posVec;
    Vector3 v2;
    struct RaycastGround rg;
    unsigned char idx394, idx395;
    void *bmd;
    void *kcl;

    if (mSpawnParam == 0xffff) {
        int *p = (int*)(((int)((char *)this) + 0xb0));
        *p = *p & ~2;
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(((char *)this), 0, 0x1000, 0, 0);
        return 1;
    }

    mBasePos = mPosX;
    unk_38c = mPosY;
    unk_390 = mPosZ;
    _ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel);

    {
        enum Bool isSpecial;
        ((char *)this)[0x395] = 0;
        isSpecial = (enum Bool)(mActorID == 0x1e);
        if (isSpecial) {
            ((char *)this)[0x395] = 2;
        } else if (data_0209f2f8 == 7) {
            ((char *)this)[0x395] = 1;
        }
    }

    ((char *)this)[0x394] = (unsigned char)(mSpawnParam & 0xf);

    if ((unsigned char)((char *)this)[0x394] == 2) {
        char *tbl = data_ov091_02134cdc;
        int s = 0x78;
        int i = (unsigned char)((char *)this)[0x395];
        Vec3_Add(&v, (Vector3*)((char *)&mBasePos), (Vector3*)(tbl + i * s));
        i = (unsigned char)((char *)this)[0x395];
        v.y += *(int*)(data_ov091_02134d1c + i * s);
        _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x1d, 0xffff, &v, 0, mAreaId, -1);
    }

    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);

    {
        char *tbl = data_ov091_02134cdc;
        int s = 0x78;
        int i = (unsigned char)((char *)this)[0x395];
        idx394 = (unsigned char)((char *)this)[0x394];
        MulVec3Mat4x3((Vector3*)(tbl + i * s + idx394 * 0xc), &data_020a0e68, &rotated);
    }

    Vec3_Add(&v2, (Vector3*)((char *)&mBasePos), &rotated);

    mPosX = v2.x;
    mPosY = v2.y;
    mPosZ = v2.z;

    idx395 = (unsigned char)((char *)this)[0x395];
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov091_02134c30 + idx395 * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, bmd, 1, -1);

    func_ov091_02131340(((char *)this));
    _ZN8Platform19UpdateClsnPosAndRotEv(((char *)this));

    idx395 = (unsigned char)((char *)this)[0x395];
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void**)((char*)data_ov091_02134c34 + idx395 * 0xc));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char *)this) + 0x124, kcl, ((char *)this) + 0x2ec, 0x199, mAngleY,
        *(void**)((char*)data_ov091_02134c38 + idx395 * 0xc));

    func_020393d4(((char *)this) + 0x124, &_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    posVec.x = mPosX;
    posVec.y = mPosY;
    posVec.z = mPosZ;
    posVec.y -= 0x14000;

    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &posVec, 0);
    unk_37c = posVec.y;
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        unk_37c = rg.f44;

    unk_380 = mAngleX;
    unk_382 = mAngleY;
    unk_384 = mAngleZ;

    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
