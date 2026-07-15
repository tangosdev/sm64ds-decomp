//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct SharedFilePtr { int id; void *file; };

extern "C" {
void *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr *f);
void *_ZN9Animation8LoadFileER13SharedFilePtr(SharedFilePtr *f);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
void LoadBlueCoinModel(void *c);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, Fix12 b, unsigned int cc);
void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *a, Vector3 *v, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
int _ZN5Actor18GetBitInDeathTableEv(void *self);

extern SharedFilePtr data_ov084_02130dfc;
extern SharedFilePtr *data_ov084_021302f4[];
extern SharedFilePtr data_ov002_0210da38;
extern SharedFilePtr data_ov084_02130df4;
}

extern "C" int _ZN19FirePiranhaPlantBig13InitResourcesEv(char *c)
{
    int i;
    Vector3 v;
    int id;
    int cond;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        c + 0x110,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130dfc),
        1, -1);

    for (i = 0; i < 6; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov084_021302f4[i]);

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da38);
    LoadBlueCoinModel(c);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        c + 0x110, data_ov084_02130df4.file, 0x40000000, 0x1000, 0);

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        c + 0x174, c, 0, 0, 0x200001, 0x66fe0);

    v.x = 0;
    v.y = 0;
    v.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x1a8, c, &v, 0x4b000, 0x64000, 0x200002, 0x66fe0);

    *(int *)(c + 0x204) = 0;
    *(int *)(c + 0x1e8) = 0;
    *(int *)(c + 0x1ec) = 0;
    *(int *)(c + 0x1f0) = 0;
    *(int *)(c + 0x1f4) = 0;
    *(unsigned char *)(c + 0x21a) = 0;
    *(unsigned char *)(c + 0x21b) = 0;
    *(unsigned char *)(c + 0x21c) = 0;
    *(unsigned char *)(c + 0x21d) = 0;
    *(unsigned char *)(c + 0x21e) = 1;
    *(int *)(c + 0x228) = 0;
    *(int *)(c + 0x224) = *(int *)(c + 0x228);

    id = *(unsigned short *)(c + 0xc);
    cond = (id == 0xfc);
    if (cond != 0) {
        *(int *)(c + 0x208) = 0x3c;
        *(int *)(c + 0x20c) = 0xaa;
        *(int *)(c + 0x210) = 0x800;
        *(int *)(c + 0x214) = 0x52;
        *(int *)(c + 0x1ec) = 1;
        *(int *)(((int)c + 0x190) & 0xFFFFFFFFFFFFFFFF) |= 0x8000;
    } else {
        cond = (id == 0xfd);
        if (cond != 0) {
            *(int *)(c + 0x208) = 0x28;
            *(int *)(c + 0x20c) = 0xaa;
            *(int *)(c + 0x210) = 0x1000;
            *(int *)(c + 0x214) = 0xa4;
            *(int *)(c + 0x1ec) = 1;
        } else {
            *(int *)(c + 0x208) = 0x28;
            *(int *)(c + 0x20c) = 0x96;
            *(int *)(c + 0x210) = 0x2000;
            *(int *)(c + 0x214) = 0x147;
            *(int *)(c + 0x1ac) = 0x64000;
            *(int *)(c + 0x1b0) = 0x64000;
            if (_ZN5Actor18GetBitInDeathTableEv(c) != 0)
                *(unsigned char *)(c + 0x220) = 0;
            else
                *(unsigned char *)(c + 0x220) = 1;
        }
    }

    *(unsigned char *)(c + 0x21f) = (unsigned char)(*(int *)(c + 8) & 0xf);
    return 1;
}
