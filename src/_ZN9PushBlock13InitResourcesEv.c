typedef int Fix12;
struct Vector3 { int x, y, z; };

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *f);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void func_ov002_020b9a1c(char *t);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *act, Fix12 a, Fix12 b, unsigned int c2, unsigned int d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *act, Fix12 a, Fix12 b, void *d, void *e);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const struct Vector3 *pos, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void func_ov002_020b9704(char *c, int i);
extern void _ZN13RaycastGroundD1Ev(void *self);

extern void *data_ov002_0210d9d0[];
extern void *data_ov002_0210d9b0[];

int _ZN9PushBlock13InitResourcesEv(char *c) {
    struct Vector3 pos;
    char ray[0x54];
    short *angp;

    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9d0);
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210d9b0);
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x124, data_ov002_0210d9b0[1], 1, -1) == 0)
        return 0;
    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, data_ov002_0210d9d0[1], 1, -1) == 0)
        return 0;
    if (_ZN11ShadowModel12InitCylinderEv(c + 0x174) == 0)
        return 0;

    *(int *)(c + 0x9c) = -0x668;
    *(int *)(c + 0xa0) = -0xf000;
    func_ov002_020b9a1c(c);

    *(int *)(c + 0x80) = 0xfa0;
    *(int *)(c + 0x84) = 0xfa0;
    *(int *)(c + 0x88) = 0xfa0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x1cc, c, 0x32000, 0x64000, 0x800002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x200, c, 0x3c000, 0x3c000, 0, 0);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x200);

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y += 0x14000;
    _ZN13RaycastGroundC1Ev(ray);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(ray, &pos, 0);
    *(int *)(c + 0x3bc) = pos.y;
    if (_ZN13RaycastGround10DetectClsnEv(ray))
        *(int *)(c + 0x3bc) = *(int *)(ray + 0x44);
    *(unsigned char *)(c + 0x3ca) = 0xb4;

    if (*(int *)(c + 8) == 0xffff) {
        if (*(int *)((char *)_ZN5Actor13ClosestPlayerEv(c) + 8) == 1 && _ZN8SaveData16HasPlayerLostCapEv() == 0) {
            func_ov002_020b9704(c, 2);
        } else {
            _ZN13RaycastGroundD1Ev(ray);
            return 0;
        }
    } else {
        func_ov002_020b9704(c, 0);
    }
    angp = (short *)(int)(((long long)(int)(c + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL);
    *angp = *angp - 0x4000;
    _ZN13RaycastGroundD1Ev(ray);
    return 1;
}
