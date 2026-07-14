//cpp
#pragma opt_common_subs off
#pragma opt_strength_reduction off

typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

struct C;
typedef int (C::*PMF)();
struct State {
    char pad[8];
    PMF fn;
};

extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern int RandomIntInternal(int *seed);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *src, void *m, void *dst);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int x, int y, int z, const void *v, void *cb);
extern void func_ov066_021194fc(char *c);
extern int _ZN16MeshColliderBase9IsEnabledEv(char *c);
extern void func_ov066_021194a4(char *c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, void *clsn);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char *c, const Vector3 *v);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern void _ZN12CylinderClsn6UpdateEv(char *c);
extern void _ZN14BlendModelAnim7AdvanceEv(char *c);
extern void _ZN9Animation7AdvanceEv(char *c);

extern int data_0209e650;
extern char data_ov066_0211b07c;
extern char data_020a0e68[];
extern int data_ov066_0211ad18[];
}

extern "C" int _ZN6Eyerok8BehaviorEv(char *c)
{
    DecIfAbove0_Short((u16 *)(c + 0x4d0));
    DecIfAbove0_Short((u16 *)(c + 0x4d2));

    {
        State *st = *(State **)(c + 0x48c);
        if (*(int *)((char *)st + 8) != 0)
            (((C *)c)->*(st->fn))();
    }

    if (*(u16 *)(c + 0x4d4) != 0) {
        if ((*(u16 *)(c + 0x4d4) & 1) == 0) {
            int rnd = RandomIntInternal(&data_0209e650);
            int off = (*(u16 *)(c + 0x4d4) >> 1) * 0xc;
            int base_dc = 0x4dc;
            int base_e4 = 0x4e4;
            char *bx = c + base_dc;
            char *bz = c + base_e4;
            char *by = c + 0x4e0;
            int *px;
            int *pz;
            int *py;
            int zero;
            Vector3 vin;
            Vector3 vout;
            *(int *)(bx + off) = *(int *)(c + 0x5c);
            *(int *)(by + off) = *(int *)(c + 0x60);
            *(int *)(bz + off) = *(int *)(c + 0x64);
            px = (int *)(bx + off);
            py = (int *)(by + off);
            pz = (int *)(bz + off);
            zero = 0;
            vin.x = zero;
            vin.y = zero;
            vin.z = zero;
            vout.x = zero;
            vout.y = zero;
            vout.z = zero;
            if (*(void **)(c + 0x48c) != (void *)&data_ov066_0211b07c) {
                if (*(s16 *)(c + 0x8e) != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(*(s16 *)(c + 0x8e) - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    if (((rnd >> 16) & 1) == 0)
                        *px += (((rnd >> 8) & 3) * 0x28) << 12;
                    else
                        *px -= (((rnd >> 8) & 3) * 0x28) << 12;
                    *pz += 0x19000;
                }
                *py += ((*(u16 *)(c + 0x4d4) * 0xa) + 0x23) << 12;
            } else {
                if (*(s16 *)(c + 0x8e) != 0) {
                    vin.z = (0x7e - (((rnd >> 8) & 0x3f) << 2)) << 12;
                    Matrix4x3_FromRotationY(data_020a0e68, (s16)(*(s16 *)(c + 0x8e) - 0x4000));
                    MulVec3Mat4x3(&vin, data_020a0e68, &vout);
                    *px += vout.x;
                    *pz += vout.z;
                } else {
                    int a = ((rnd >> 24) & 7) * 0x1e;
                    int b = ((rnd >> 16) & 7) * 0x1e;
                    *pz -= 0x64000;
                    *px += (0x69 - a) << 12;
                    *pz += (0x69 - b) << 12;
                }
                *py += 0x96000;
            }
        }

        {
            int i = 0;
            char *cur = c;
            u32 id0 = 0x13a;
            u32 id1 = 0x13b;
            int z0 = 0;
            for (; i < 0x14; i++) {
                if (*(int *)(cur + 0x4dc) != 0 || *(int *)(cur + 0x4e0) != 0 || *(int *)(cur + 0x4e4) != 0) {
                    *(u32 *)(c + 0x5cc + (i << 2)) =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            *(u32 *)(c + 0x5cc + (i << 2)), id0,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                    *(u32 *)(c + 0x61c + (i << 2)) =
                        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                            *(u32 *)(c + 0x61c + (i << 2)), id1,
                            *(int *)(cur + 0x4dc), *(int *)(cur + 0x4e0), *(int *)(cur + 0x4e4),
                            (void *)z0, (void *)z0);
                }
                cur += 0xc;
            }
        }

        {
            int o4d4 = 0x4d4;
            u16 *p = (u16 *)(((long long)(int)(c + o4d4)) & 0xFFFFFFFFFFFFFFFFLL);
            u16 v = *p;
            char *c400 = c + 0x400;
            *p = (u16)(v + 1);
            if (*(u16 *)(c400 + 0xd4) > 0x26) {
                int j = 0;
                char *q = c;
                *(u16 *)(c400 + 0xd4) = (u16)j;
                for (; j < 0x14; j++) {
                    *(int *)(q + 0x4dc) = 0;
                    *(int *)(q + 0x4e0) = 0;
                    *(int *)(q + 0x4e4) = 0;
                    q += 0xc;
                }
            }
        }
    }

    if (*(int *)(c + 0x49c) == 0) {
        func_ov066_021194fc(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
            func_ov066_021194a4(c);
        return 1;
    }

    {
        Vector3 vrel;
        *(int *)(c + 0x4a8) = *(int *)(c + 0x4b4) + 0x8000;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
        *(int *)(c + 0x354) = *(int *)(c + 0x5c);
        *(int *)(c + 0x358) = *(int *)(c + 0x60);
        *(int *)(c + 0x35c) = *(int *)(c + 0x64);
        vrel.x = data_ov066_0211ad18[0];
        vrel.y = data_ov066_0211ad18[1];
        vrel.z = data_ov066_0211ad18[2];
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x320, &vrel);
        func_ov066_021194fc(c);
        if (_ZN16MeshColliderBase9IsEnabledEv(c + 0x674) != 0)
            func_ov066_021194a4(c);
        _ZN12CylinderClsn5ClearEv(c + 0x320);
        _ZN12CylinderClsn6UpdateEv(c + 0x320);
        _ZN14BlendModelAnim7AdvanceEv(c + 0x360);
        _ZN9Animation7AdvanceEv(c + 0x448);
    }
    return 1;
}
