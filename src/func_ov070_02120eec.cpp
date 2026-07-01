//cpp
struct SharedFilePtr;
struct BMD_File;
struct BTP_File;
struct BTA_File;
struct Actor;
struct Vector3;
struct Vector3_16;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" BTP_File *_ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(BMD_File &a, BTP_File &b);
extern "C" void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(BMD_File &a, BTA_File &b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(Actor *self, int a, int b, int c, int d);
extern "C" void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, Actor *a, Vector3 const &b, int c, int d, unsigned int e, unsigned int f);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, Actor *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" int func_ov070_02120da8(void *c, int a);
extern "C" void func_ov070_02120724(char *c);

extern SharedFilePtr data_ov070_021235fc;
extern SharedFilePtr data_ov070_02123604;
extern SharedFilePtr *data_ov070_021222e0[];
extern SharedFilePtr data_ov070_021235ec;
extern BTA_File data_ov070_021231f4;
extern Vector3 data_ov070_0212365c;
extern char data_02082128;

struct M48 { int w[12]; };

extern "C" int func_ov070_02120eec(char *c)
{
    BMD_File *bmd;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov070_021235fc);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, 1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov070_02123604);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x138, bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        _ZN9Animation8LoadFileER13SharedFilePtr(*data_ov070_021222e0[i]);
    }

    BMD_File *bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    BTP_File *btp = _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov070_021235ec);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*bmd2, *btp);

    bmd2 = *(BMD_File **)((char *)&data_ov070_02123604 + 4);
    _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*bmd2, data_ov070_021231f4);

    if (!_ZN11ShadowModel12InitCylinderEv(c + 0x1b0))
        return 0;

    if ((unsigned char)((*(unsigned int *)(c + 8) >> 1) & 1)) {
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_((Actor *)c, 0, 0x20d000, 0x1000000, 0xa28000);
    } else {
        _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_((Actor *)c, 0, 0x2c1000, 0x1000000, 0xa28000);
    }

    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x1d8, (Actor *)c, data_ov070_0212365c, 0x2d000, 0x50000, 0x200002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x218, (Actor *)c, 0x2d000, 0x2d000, 0, 0);

    *(int *)(c + 0x9c) = 0;
    *(int *)(c + 0xa0) = 0;
    func_ov070_02120da8(c, 1);

    *(M48 *)(c + 0x3d4) = *(M48 *)&data_02082128;

    func_ov070_02120724(c);
    return 1;
}
