//cpp
struct SharedFilePtr;
struct BMD_File;
struct Actor;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" int _ZN11ShadowModel12InitCylinderEv(void *self);
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, Actor *a, int b, int c, unsigned int d, unsigned int e);
extern "C" void func_ov002_020b2c44(void *self);
extern "C" void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, Actor *a, int b, int c, void *d, int e);

extern SharedFilePtr data_ov002_0210da58;
extern unsigned char data_0209f2d8;

struct Sub { char pad[0x7c]; short f7c; };
extern char *data_0209f318;


extern "C" int _ZN11WingFeather13InitResourcesEv(char *self)
{
    BMD_File *bmd;
    int b;

    bmd = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da58);
    if (!_ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, 1))
        return 0;
    if (!_ZN11ShadowModel12InitCylinderEv(self + 0x314))
        return 0;

    *(int *)(self + 0x9c) = -0x199;
    *(int *)(self + 0xa0) = -0x28000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(self + 0x124, (Actor *)self, 0x6e000, 0x6e000, 0x100002, 0);
    func_ov002_020b2c44(self);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x158, (Actor *)self, 0x28000, 0xa000, 0, 0);

    *(short *)(self + 0x8c) = 0x4000;
    *(short *)(self + 0x90) = -0x4000;
    b = (data_0209f2d8 == 1);
    if (!b) {
        *(short *)(self + 0x8e) = ((Sub *)(data_0209f318 + 0x100))->f7c + 0x4000;
    }
    *(unsigned char *)(self + 0x384) = 0xb4;
    *(int *)(self + 0xa8) = 0x3000;
    return 1;
}
