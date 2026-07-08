typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef signed char s8;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *m, void *f, int a, int b);
extern void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
extern void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *tt, void *bta, int a, int fix, unsigned int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void *c);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void *c);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *sfp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *mc, void *kcl, void *mtx, int fix, short s, void *clps);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void *mc, void *actor);

extern void *data_ov029_021142fc[];
extern void *data_ov029_02112fb8;
extern u8 LEVEL_SPECIFIC_SETTING[];
extern int data_ov029_02112b2c[];
extern int WATER_HEIGHT;
extern void *data_ov029_02114304[];
extern void *data_ov029_02112fec;

int func_ov029_021124d0(char *c)
{
    void *m;
    void *k;

    m = _ZN5Model8LoadFileER13SharedFilePtr(data_ov029_021142fc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, m, 1, -1);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(data_ov029_021142fc[1], &data_ov029_02112fb8);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(c + 0x320, &data_ov029_02112fb8, 0, 0x1000, 0);

    *(u8*)(c + 0x338) = (u8)(*(int*)(c + 8) & 1);
    if (*(u8*)(c + 0x338) == 0) {
        unsigned int idx = LEVEL_SPECIFIC_SETTING[0];
        if (idx >= 3) idx = 2;
        *(int*)(c + 0x60) = data_ov029_02112b2c[idx];
    }
    *(int*)(c + 0x334) = *(int*)(c + 0x60);
    WATER_HEIGHT = *(int*)(c + 0x60);

    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    k = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov029_02114304);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, k, c + 0x2ec, 0x1000, *(s16*)(c + 0x8e), &data_ov029_02112fec);
    _ZN16MeshColliderBase6EnableEP5Actor(c + 0x124, c);

    *(u8*)(c + 0x340) = (u8)*(s8*)(c + 0xcc);
    *(s8*)(c + 0xcc) = -1;
    *(s16*)(c + 0x300 + 0x42) = 0;

    return 1;
}
