// NONMATCHING: base materialization / addressing (div=39). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
struct Actor; struct Vector3; struct Vector3_16; struct BMD_File;
typedef struct { int w[2]; } SharedFilePtr;

extern struct BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* fp);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr* fp);
extern void LoadBlueCoinModel(void* c);
extern int _ZN11ShadowModel12InitCylinderEv(char* self);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(char* self, struct Actor* a, int r, int h, struct Vector3_16* rot, int f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(char* self, struct BMD_File* f, int a, int b);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(char* self, struct Actor* a, struct Vector3* pos, int r, int h, u32 f1, u32 f2);

extern SharedFilePtr data_ov020_02114aa0;
extern SharedFilePtr data_ov020_02114ab8;
extern SharedFilePtr data_ov020_02114aa8;
extern SharedFilePtr data_ov020_02114ab0;

struct M48 { int w[12]; };
extern struct M48 _ZN9Matrix3x38IDENTITYE;

int func_ov020_021124e4(char* c)
{
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov020_02114aa0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov020_02114ab8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov020_02114aa8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov020_02114ab0);
    LoadBlueCoinModel(c);

    if (_ZN11ShadowModel12InitCylinderEv(c+0x1c4) == 0)
        return 0;

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c+0x25c, (struct Actor*)c, 0x32000, 0x32000, 0, 0);

    *(int*)(c+0x438) = 0;
    *(int*)(c+0x43c) = 0;
    *(int*)(c+0x440) = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c+0x21c, (struct Actor*)c, (struct Vector3*)(c+0x438), 0x19000, 0x32000, 0x200001, 0);

    *(int*)(c+0x418) = 0;
    *(int*)(c+0x41c) = 0;
    *(int*)(c+0x80) = 0x800;
    *(int*)(c+0x84) = 0x800;
    *(int*)(c+0x88) = 0x800;
    *(int*)(c+0x42c) = *(int*)(c+0x5c);
    *(int*)(c+0x430) = *(int*)(c+0x60);
    *(int*)(c+0x434) = *(int*)(c+0x64);

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c+0x174, (struct BMD_File*)data_ov020_02114ab8.w[1], 1, -1) == 0)
        return 0;

    *(struct M48*)(c+0x1ec) = _ZN9Matrix3x38IDENTITYE;
    *(char*)(c+0x450) = 0;
    *(short*)(c+0x8e) = *(short*)(c+0x8e) + 0x8000;
    *(int*)(c+0x44c) = 0x800;

    {
        unsigned short id = *(unsigned short*)(c+0xc);
        unsigned int match1 = (id == 0x145);

        if (match1)
        {
            *(int*)(c+0x420) = 0;
            *(int*)(c+0x424) = 4;
            *(unsigned char*)(c+0x108) = 0;
            *(int*)(c+0x238) |= 0x26fe0;
            goto success;
        }
        {
            unsigned int match2 = (id == 0x147);
            if (match2)
            {
                *(int*)(c+0x420) = 0;
                *(int*)(c+0x424) = 0;
                *(unsigned char*)(c+0x108) = 3;
                *(int*)(c+0x238) |= 0x2efe0;
                goto success;
            }
        }
        {
            unsigned int match3 = (id == 0xd5);
            if (match3)
            {
                *(int*)(c+0x420) = 1;
                *(int*)(c+0x424) = 6;
                *(unsigned char*)(c+0x108) = 0;
                *(int*)(c+0x234) |= 4;
                *(int*)(c+0x238) |= 0x3c0;
                *(int*)(c+0x80) = 0x1000;
                *(int*)(c+0x84) = 0x800;
                *(int*)(c+0x88) = 0x800;
                goto success;
            }
        }
    }
    return 0;

success:
    return 1;
}
