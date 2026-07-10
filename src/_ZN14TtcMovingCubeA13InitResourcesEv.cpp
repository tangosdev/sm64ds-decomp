//cpp
typedef short s16;

struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Actor;
struct Matrix4x3;
struct CLPS_Block;

struct Vector3 { int x, y, z; };

struct RaycastGround {
    char pad[0x44];
    int result;       /* 0x44 */
    char pad2[0x54 - 0x48];
    RaycastGround();
    ~RaycastGround();
    int SetObjAndPos(const Vector3 &p, Actor *a);
    int DetectClsn();
};

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" int _ZN11ShadowModel10InitCuboidEv(void *self);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *self, KCL_File *f, const Matrix4x3 &m, int fx, s16 s, CLPS_Block &b);

extern SharedFilePtr data_ov065_0211d9d4;
extern SharedFilePtr data_ov065_0211d9cc;
extern CLPS_Block func_02112118;

extern "C" int _ZN14TtcMovingCubeA13InitResourcesEv(char *c)
{
    BMD_File *bmd;
    KCL_File *kcl;
    Vector3 v;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov065_0211d9d4);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, -1);
    _ZN11ShadowModel10InitCuboidEv(c + 0x334);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
    kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov065_0211d9cc);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, kcl, *(Matrix4x3 *)(c + 0x2ec), 0x1000, *(s16 *)(c + 0x8e), func_02112118);

    *(int *)(c + 0x320) = *(int *)(c + 0x60);
    {
        int t = *(int *)(c + 8) & 0xff;
        if (t != 0xff && t != 0)
            *(int *)(c + 0x324) = t * 0xa000 + *(int *)(c + 0x320);
        else
            *(int *)(c + 0x324) = *(int *)(c + 0x320) + 0x1f4000;
    }
    *(int *)(c + 0xa0) = -0x3c000;
    *(int *)(c + 0xa8) = 0x6000;
    *(unsigned char *)(c + 0x328) = 1;

    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y -= 0xa000;
    RaycastGround rc;
    rc.SetObjAndPos(v, 0);
    *(int *)(c + 0x330) = v.y;
    if (rc.DetectClsn())
        *(int *)(c + 0x330) = rc.result;
    return 1;
}
