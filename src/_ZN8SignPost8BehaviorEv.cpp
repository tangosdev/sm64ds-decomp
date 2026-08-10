//cpp
typedef unsigned int u32;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };
enum Bool { FALSE, TRUE };
#define LD(p) ((int)(p))

extern "C" unsigned int data_0209b454;

extern "C" void _ZN6Player9DropActorEv(void *self);
extern "C" void func_ov002_020bb060(char *c);
extern "C" u8 DecIfAbove0_Byte(u8 *p);
extern "C" int _ZN5Actor13DistToCPlayerEv(void *self);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN8Platform19UpdateClsnPosAndRotEv(void *self);
extern "C" void func_ov002_020baf80(char *t);
extern "C" int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void *self, short a, short b, short c, int fix);
extern "C" int _ZN16MeshColliderBase9IsEnabledEv(void *self);
extern "C" void _ZN16MeshColliderBase7DisableEv(void *self);
extern "C" void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void *v, void *cb);
extern "C" void *_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, int c, int d, int e, const void *v);
extern "C" void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void *self, const struct Vector3 *vec);
extern "C" void func_ov002_020bae9c(char *c);
extern "C" void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *m, int a, int b, int c, u32 j);
extern "C" void _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern "C" void func_ov002_020bbda4(char *c);
extern "C" void _ZN12CylinderClsn5ClearEv(void *self);
extern "C" void _ZN12CylinderClsn6UpdateEv(void *self);
extern "C" void func_ov002_020bafc0(char *self);

extern "C" int _ZN8SignPost8BehaviorEv(char *c)
{
    struct Vector3 v;
    struct Vector3 vec, vec2;

    {
        enum Bool b = (enum Bool)((*(u32 *)(c + 0xb0) & 0x4000000) != 0);
        if (b != FALSE && (data_0209b454 & 0x4000000) && *(void **)(c + 0x59c) != 0)
            _ZN6Player9DropActorEv(*(void **)(c + 0x59c));
    }

    {
        void *p = *(void **)(c + 0x59c);
        if (p != 0 && (enum Bool)((*(u32 *)(c + 0xb0) & 0x4000) != 0) != FALSE
            && *(int *)((char *)p + 0xc8) != 0) {
            u32 *fp;
            func_ov002_020bb060(c);
            *(int *)(c + 0xb8) = 0x20000;
            fp = (u32 *)LD(c + 0xb0);
            *fp = *fp | 0x4000000;
        } else {
            u32 *fp;
            *(int *)(c + 0xb8) = 0x10000;
            fp = (u32 *)LD(c + 0xb0);
            *fp = *fp & ~0x4000000;
        }
    }

    if (*(u8 *)(c + 0x590) != 0) {
        enum Bool b = (enum Bool)((*(u32 *)(c + 0xb0) & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte((u8 *)(c + 0x591)) == 0
            && _ZN5Actor13DistToCPlayerEv(c) > 0x7d0000)
            *(u8 *)(c + 0x590) = 0;
        return 1;
    }

    if (*(u8 *)(c + 0x58e) == 0) {
        enum Bool b = (enum Bool)((*(u32 *)(c + 0xb0) & 8) != 0);
        if (b != FALSE && DecIfAbove0_Byte((u8 *)(c + 0x591)) == 0
            && _ZN5Actor13DistToCPlayerEv(c) > 0x7d0000) {
            *(u8 *)(c + 0x58e) = 2;
            *(int *)(c + 0x60) = *(int *)(c + 0x3b4);
            _ZN8Platform21UpdateModelPosAndRotYEv(c);
            _ZN8Platform19UpdateClsnPosAndRotEv(c);
            func_ov002_020baf80(c);
        }
    }

    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(c, -0x2000, 0, 0, 0x46000))
        return 1;

    if (*(u8 *)(c + 0x58c) != 0) {
        int x, y, z;
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x124))
            _ZN16MeshColliderBase7DisableEv(c + 0x124);
        x = *(int *)(c + 0x5c);
        z = *(int *)(c + 0x64);
        y = *(int *)(c + 0x60) + 0x50000;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y;
        ((int *)&v)[2] = z;
        if (DecIfAbove0_Byte((u8 *)(c + 0x58c)) != 0) {
            *(void **)(c + 0x584) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(u32 *)(c + 0x584), 0x13a, v.x, v.y, v.z, 0, 0);
            *(void **)(c + 0x588) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(u32 *)(c + 0x588), 0x13b, v.x, v.y, v.z, 0);
        } else {
            int x2, y2, z2;
            x2 = *(int *)(c + 0x5c);
            z2 = *(int *)(c + 0x64);
            y2 = *(int *)(c + 0x60) + 0x28000;
            ((int *)&vec)[0] = x2;
            ((int *)&vec)[1] = y2;
            ((int *)&vec)[2] = z2;
            vec2 = vec;
            _ZN5Actor19DisappearPoofDustAtERK7Vector3(c, &vec2);
            func_ov002_020bae9c(c);
            return 1;
        }
        _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            c, c + 0x358, c + 0x380, 0x50000, 0x28000, 0x28000, 0xf);
        return 1;
    }

    DecIfAbove0_Byte((u8 *)(c + 0x58f));
    if (*(u8 *)(c + 0x590) == 0)
        _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0);
    func_ov002_020bbda4(c);
    _ZN12CylinderClsn5ClearEv(c + 0x320);
    _ZN12CylinderClsn6UpdateEv(c + 0x320);
    {
        int s = *(int *)(c + 0x354);
        if (s == 3) {
            func_ov002_020bafc0(c);
        } else if (*(u8 *)(c + 0x58e) == 2 && (u32)s <= 1) {
            _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
                c, c + 0x358, c + 0x380, 0x50000, 0x28000, 0x28000, 0xf);
        }
    }
    return 1;
}
