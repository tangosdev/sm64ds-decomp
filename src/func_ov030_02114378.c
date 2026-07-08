#define false 0
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *file, int idx, int speed, u32 flags);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *self, void *actor, int a, int b, u32 c, u32 d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v0, void *v1);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern char *_ZN5Actor13ClosestPlayerEv(void *self);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, void *pos, void *rot, int a, int b);
extern void func_ov030_021141a8(void *self, int a);
extern void func_ov030_02112094(void *self);

extern char MARIO_CAP_MODEL_PTR;
extern char LUIGI_CAP_MODEL_PTR;
extern char WARIO_CAP_MODEL_PTR;
extern char data_ov030_02115d00;
extern void *data_ov030_02114824[];
extern void *data_ov030_02115cf0[];

int func_ov030_02114378(char *s)
{
    int i;
    int b;
    u16 h;
    int *py;

    _ZN5Model8LoadFileER13SharedFilePtr(&MARIO_CAP_MODEL_PTR);
    _ZN5Model8LoadFileER13SharedFilePtr(&LUIGI_CAP_MODEL_PTR);
    _ZN5Model8LoadFileER13SharedFilePtr(&WARIO_CAP_MODEL_PTR);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(s + 0xd4, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov030_02115d00), 1, 1);
    for (i = 0; i < 10; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov030_02114824[i]);
    if (_ZN11ShadowModel12InitCylinderEv(s + 0x138) == 0)
        return 0;
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(s + 0xd4, data_ov030_02115cf0[1], 0, 0x1000, 0);
    *(int *)(s + 0x130) = 0x1000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(s + 0x160, s, 0x28000, 0x64000, 0x800004, 0x49000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(s + 0x194, s, 0x32000, 0x32000, 0, 0);
    *(int *)(s + 0x9c) = -0x2000;
    *(int *)(s + 0xa0) = -0x3c000;
    py = (int *)(((int)s + 0x384) & 0xFFFFFFFFFFFFFFFFLL);
    *(int *)(s + 0x380) = *(int *)(s + 0x5c);
    *(int *)(s + 0x384) = *(int *)(s + 0x60);
    *(int *)(s + 0x388) = *(int *)(s + 0x64);
    *py += 0x64000;
    *(int *)(s + 0x38c) = *(int *)(s + 0x5c);
    *(int *)(s + 0x390) = *(int *)(s + 0x60);
    *(int *)(s + 0x394) = *(int *)(s + 0x64);
    *(int *)(s + 0x80) = 0x1000;
    *(int *)(s + 0x84) = 0x1000;
    *(int *)(s + 0x88) = 0x1000;
    *(int *)(s + 0x3a8) = 0;
    *(int *)(s + 0x3ac) = 0;
    *(u8 *)(s + 0x3cb) = 0x96;
    h = *(u16 *)(s + 0xc);
    b = 0;
    if (h == 0x10b)
        b = 1;
    if (b != 0) {
        if (_ZN8SaveData16HasPlayerLostCapEv() != 0) {
            char *player = _ZN5Actor13ClosestPlayerEv(s);
            if (*(u32 *)(player + 8) >= 3)
                goto ov030_no_spawn;
            {
                char *spawned;
                *(u32 *)(s + 0x3b0) = *(u32 *)(player + 8);
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, (*(u32 *)(s + 0x3b0) << 8) | 2, s + 0x5c, 0, *(s8 *)(s + 0xcc), -1);
                *(int *)(s + 0x3ac) = *(int *)(spawned + 4);
                *(u8 *)(s + 0x3c8) = 1;
                func_ov030_021141a8(s, 1);
                goto ov030_cap_done;
            }
ov030_no_spawn:
            func_ov030_021141a8(s, 0);
ov030_cap_done:
            ;
        } else {
            func_ov030_021141a8(s, 0);
        }
    } else {
        int t = (int)(h == 0x10c);
        if (t != 0)
            func_ov030_021141a8(s, 1);
    }
    func_ov030_02112094(s);
    return 1;
}