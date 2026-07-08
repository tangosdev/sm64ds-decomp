//cpp
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct Actor;
struct Vector3_16;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" BCA_File *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, BCA_File *f, int a, int b, unsigned int c);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, Actor *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, Actor *a, int b, int c, Vector3_16 *d, Vector3_16 *e);
extern "C" int func_ov077_02125e94(void *c, int a, int b);
extern "C" void func_ov077_02125304(char *c);

extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;
extern char _ZN9Matrix3x38IDENTITYE;

struct M48 { int w[12]; };

extern "C" int func_ov077_02126058(char *c)
{
    BMD_File *bmd;
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b48);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, bmd, 1, -1);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov077_02127b38);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x124, bmd, 1, -1);
    _ZN9Animation8LoadFileER13SharedFilePtr(data_ov077_02127c14);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x124, *(BCA_File **)((char *)&data_ov077_02127c14 + 4), 0, 0x1000, 0);
    if (!_ZN11ShadowModel12InitCylinderEv(c + 0x188))
        return 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x1b0, (Actor *)c, 0x2d000, 0x3c000, 0x200000, 0x4a3d0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1e4, (Actor *)c, 0x2d000, 0, (Vector3_16 *)(c + 0x92), (Vector3_16 *)(c + 0x8c));
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(unsigned char *)(c + 0x3e9) = 0x2c;
    func_ov077_02125e94(c, 0, 0x2c);
    *(M48 *)(c + 0x3a0) = *(M48 *)&_ZN9Matrix3x38IDENTITYE;
    func_ov077_02125304(c);
    return 1;
}
