typedef int Fix12;
typedef struct { int w[2]; } SharedFilePtr;
typedef struct { short x,y,z; } Vector3_16;
typedef struct BMD_File BMD_File;
typedef struct Actor Actor;

extern SharedFilePtr* data_ov062_0211cee8[9];
extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, BMD_File* f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* self);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, Actor* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, Actor* a, Fix12 r, Fix12 h, Vector3_16* p, Fix12 q);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void* self);
extern void func_ov062_02117994(char* c, int idx);
extern void LoadBlueCoinModel(void* c);

int _ZN5Koopa13InitResourcesEv(char* c)
{
    int i;
    Fix12 r, h;
    int kind;
    BMD_File* f;
    unsigned int isSpecial;

    kind = *(int*)(c + 8) & 1;
    *(int*)(c + 0x394) = kind;

    for (i = 0; i < 9; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov062_0211cee8[i]);

    f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211ced8[*(int*)(c + 0x394)]);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, f, 1, -1) == 0)
        return 0;

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x364) == 0)
        return 0;

    isSpecial = *(unsigned short*)(c + 0xc) == 0xcc;
    if (isSpecial)
    {
        *(int*)(c + 0x390) = 2;
        *(int*)(c + 0x3bc) = 0x2000;
        *(int*)(c + 0x80) = 0x599;
        *(int*)(c + 0x84) = 0x599;
        *(int*)(c + 0x88) = 0x599;
        r = 0x1e000;
        h = 0x32000;
    }
    else
    {
        *(int*)(c + 0x390) = 0;
        *(int*)(c + 0x3bc) = 0x1000;
        *(int*)(c + 0x80) = 0xa66;
        *(int*)(c + 0x84) = 0xa66;
        *(int*)(c + 0x88) = 0xa66;
        r = 0x3c000;
        h = 0x64000;
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov062_0211cee0[*(int*)(c + 0x394)]);
    }

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, (Actor*)c, r, h, 0x200000, 0xb6efe0);

    *(short*)(c + 0x100) = 0;
    *(short*)(c + 0x300 + 0xc4) = 0;
    *(int*)(c + 0x38c) = 1;
    func_ov062_02117994(c, 0);

    *(unsigned char*)(c + 0x106) = 0;
    *(short*)(c + 0x300 + 0xca) = 0;
    *(int*)(c + 0x39c) = *(int*)(c + 0x5c);
    *(int*)(c + 0x3a0) = *(int*)(c + 0x60);
    *(int*)(c + 0x3a4) = *(int*)(c + 0x64);
    *(int*)(c + 0x9c) = -0x2000;
    *(int*)(c + 0xa0) = -0x3c000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, (Actor*)c, 0x32000, 0x32000, 0, 0);

    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x144);

    *(unsigned char*)(c + 0x108) = 3;
    LoadBlueCoinModel(c);

    return 1;
}
