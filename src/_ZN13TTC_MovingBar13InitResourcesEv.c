extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmd, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern void func_020393d4(int *p, int v);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;

extern char data_ov065_0211d35c[];
extern char data_ov065_0211d360[];
extern char data_ov065_0211d364[];

struct Vec3 { int x, y, z; };

int _ZN13TTC_MovingBar13InitResourcesEv(char *c)
{
    struct Vec3 pos;
    char raycast[0x50];
    int i;

    if (*(unsigned short *)(c + 0xc) != 0x72) {
        if (*(unsigned short *)(c + 0xc) == 0x73)
            *(unsigned char *)(c + 0x31e) = 1;
    } else {
        *(unsigned char *)(c + 0x31e) = 0;
    }

    i = *(unsigned char *)(c + 0x31e);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d35c + i * 0xc)),
        1, -1);

    _ZN11ShadowModel12InitCylinderEv(c + 0x324);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    i = *(unsigned char *)(c + 0x31e);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(void **)(data_ov065_0211d360 + i * 0xc)),
        c + 0x2ec,
        0x199,
        *(short *)(c + 0x8e),
        *(void **)(data_ov065_0211d364 + i * 0xc));

    func_020393d4((int *)(c + 0x124), (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y = pos.y - 0xa000;

    _ZN13RaycastGroundC1Ev(raycast);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(raycast, &pos, (void *)0);
    *(int *)(c + 0x320) = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(raycast))
        *(int *)(c + 0x320) = *(int *)(raycast + 0x44);
    _ZN13RaycastGroundD1Ev(raycast);

    return 1;
}
