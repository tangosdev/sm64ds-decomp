typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef signed char s8;

struct Vec3 { int x, y, z; };

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN11ShadowModel12InitCylinderEv(void *self);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN5Actor18GetBitInDeathTableEv(void *self);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const void *v, const void *v16, int e, int f);
extern void _ZN7PathPtrC1Ev(void *self);
extern void _ZN7PathPtr6FromIDEj(void *self, u32 id);
extern int _ZNK7PathPtr8NumNodesEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, const void *v, int d, int e, u32 f, u32 g);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, int b, int c, void *v16, int e);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void *self, void *v, u32 idx);
extern void func_ov081_02125488(void *c, void *p);

extern void *data_ov081_02128db0;
extern void *data_ov081_02128d90;
extern void *data_ov081_02128d98;
extern void *data_ov081_02128db8;
extern void *data_ov081_02128da8;
extern void *data_ov081_02128d88;
extern void *data_ov081_02128da0;
extern struct Vec3 data_ov081_02128998;
extern void *data_ov081_02128e54;
extern void *data_ov081_02128e84;

int func_ov081_02125ba0(char *c)
{
    struct Vec3 v;

    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x30c,
        _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128db0), 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov081_02128d90);
    _ZN11ShadowModel12InitCylinderEv(c + 0x370);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128d98);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128db8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128da8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128d88);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov081_02128da0);

    *(s32 *)(c + 0x418) = *(s32 *)(c + 8) & 0xff;
    *(s32 *)(c + 0x41c) = (*(s32 *)(c + 8) & 0xff00) >> 8;
    if (*(s32 *)(c + 0x418) == 0xff)
        *(s32 *)(c + 0x418) = 0;
    if (*(s32 *)(c + 0x41c) == 0xff)
        *(s32 *)(c + 0x41c) = 0;

    *(s32 *)(c + 0x44c) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x450) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x454) = *(s32 *)(c + 0x64);

    if (_ZN5Actor18GetBitInDeathTableEv(c) != 0) {
        if (*(s32 *)(c + 0x41c) == 2) {
            if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdf, 0x200,
                    c + 0x44c, 0, *(s8 *)(c + 0xcc), -1);
            } else {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdf, 0x300,
                    c + 0x44c, 0, *(s8 *)(c + 0xcc), -1);
            }
        }
        if (*(s32 *)(c + 0x41c) != 3)
            return 0;
    }

    *(s32 *)(c + 0x3fc) = 0;
    *(s32 *)(c + 0x400) = 0;

    if (*(s32 *)(c + 0x41c) == 0) {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, *(u32 *)(c + 0x418));
        *(s32 *)(c + 0x420) = _ZNK7PathPtr8NumNodesEv(pp);
    }

    *(s32 *)(c + 0x80) = 0x1000;
    *(s32 *)(c + 0x84) = 0x1000;
    *(s32 *)(c + 0x88) = 0x1000;
    *(s32 *)(c + 0xa0) = -0xc8000;

    v.x = data_ov081_02128998.x;
    v.y = data_ov081_02128998.y;
    v.z = data_ov081_02128998.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        c + 0x110, c, &v, 0x3c000, 0x96000, 0x200004, 0x42050);

    *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
    *(s16 *)(c + 0x414) = *(s16 *)(c + 0x8e);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        c + 0x150, c, 0x14000, 0xf000, 0, 0);

    *(u8 *)(c + 0x108) = 1;
    *(u8 *)(c + 0x10a) = 2;

    if (*(s32 *)(c + 0x41c) == 0) {
        char pp[8];
        _ZN7PathPtrC1Ev(pp);
        _ZN7PathPtr6FromIDEj(pp, *(u32 *)(c + 0x418));
        _ZNK7PathPtr7GetNodeER7Vector3j(pp, c + 0x5c, *(u32 *)(c + 0x424));
        *(s32 *)(c + 0x424) = 1;
        *(s32 *)(c + 0x9c) = -0x2000;
        *(s32 *)(c + 0xb0) = 0x10000000;
        func_ov081_02125488(c, &data_ov081_02128e54);
    } else {
        *(s32 *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) -= 0x118000;
        func_ov081_02125488(c, &data_ov081_02128e84);
        if (*(s32 *)(c + 0x41c) == 3) {
            *(u8 *)(c + 0x108) = 0;
            *(u8 *)(c + 0x10a) = 0;
            *(s32 *)(c + 0xb0) = 2;
        }
        if (*(s32 *)(c + 0x41c) == 2)
            *(s32 *)(c + 0xb0) = 0x8000002;
        else
            *(s32 *)(c + 0xb0) = 3;
    }

    return 1;
}
