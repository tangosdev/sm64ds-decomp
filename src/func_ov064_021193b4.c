#include "types.h"
struct Vec3 { int x, y, z; };
struct Vec3_16 { s16 x, y, z; };

extern char *_ZN5Actor13ClosestPlayerEv(void *c);
extern int _ZN5Actor13DistToCPlayerEv(void *c);
extern s16 _ZN5Actor18HorzAngleToCPlayerEv(void *c);
extern void Matrix4x3_FromRotationY(void *m, s16 angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, s16 angX);
extern void MulVec3Mat4x3(struct Vec3 *in, void *m, struct Vec3 *out);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    int a, int b, int x, int y, int z, int f, int cb);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    unsigned int a, unsigned int b, struct Vec3 *pos, struct Vec3_16 *rot, int e, int f);
extern void func_02012790(int a);
extern void _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(
    void *self, struct Vec3 *pos, unsigned int a, int b, unsigned short c, void *d);

extern int data_020a0e68;

#define M(p) ((long long)(int)(p))

int func_ov064_021193b4(char *c)
{
    struct Vec3_16 rot;
    struct Vec3 base;
    struct Vec3 off;
    struct Vec3 spos;
    struct Vec3 npos1;
    struct Vec3 npos2;
    char *p;
    int dist;
    char *spawned;
    int i;
    char *hit;
    int uid;
    int *pp;
    int px, py, pz, nx, ny, nz;
    int *p370;
    int *p30c;
    int *p374;
    int *p310;
    int *posp;
    u16 ax, ay, az;
    s16 tx;
    int m1;

    p = _ZN5Actor13ClosestPlayerEv(c);
    if (p != 0 && *(u8 *)(p + 0x6f9) == 0) {
        dist = _ZN5Actor13DistToCPlayerEv(c);
        if (dist < 0x3e8000) {
            dist = (0x3e8000 - dist) / 30;
            base.x = 0;
            base.y = 0;
            base.z = dist;
            off.x = 0;
            off.y = 0;
            off.z = 0;
            Matrix4x3_FromRotationY(&data_020a0e68, (s16)(_ZN5Actor18HorzAngleToCPlayerEv(c) + 0x8000));
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
            MulVec3Mat4x3(&base, &data_020a0e68, &off);
            pp = (int *)(int)M(p + 0x5c);
            px = pp[0];
            base.x = px;
            py = pp[1];
            base.y = py;
            pz = pp[2];
            base.z = pz;
            {
                int tnx = px + off.x;
                base.y = py + off.y;
                base.z = pz + off.z;
                base.x = tnx;
                *(int *)(p + 0x5c) = tnx;
                *(int *)(p + 0x60) = base.y;
                *(int *)(p + 0x64) = base.z;
            }
        }
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        _ZN12CylinderClsn6UpdateEv(c + 0x110);
        if (dist > 0x7d0000 && *(int *)(c + 0x30c) < 5) {
            *(int *)(c + 0x30c) = 0;
        }
    }

    if (*(int *)(c + 0x318) == 0) {
        *(int *)(c + 0x304) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x304), 0x138,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
        if (*(int *)(c + 0x30c) < 5) {
            if (*(u16 *)(c + 0x100) == 0) {
                ax = *(volatile u16 *)(c + 0x8c);
                ay = *(volatile u16 *)(c + 0x8e);
                {
                    volatile struct Vec3_16 *vr = &rot;
                    vr->x = ax;
                    vr->y = ay;
                    az = *(u16 *)(c + 0x90);
                    tx = (s16)vr->x;
                    m1 = -1;
                    vr->z = az;
                    tx = (s16)(tx + 0x4000);
                    vr->x = (u16)tx;
                }
                spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0xf4, 2, (struct Vec3 *)(c + 0x5c), &rot,
                    *(signed char *)(c + 0xcc), -1);
                if (spawned != 0) {
                    *(int *)(c + 0x320 + (*(int *)(c + 0x370) * 4)) = *(int *)(spawned + 4);
                    p370 = (int *)(int)M(c + 0x370);
                    *p370 = *p370 + 1;
                    if (*(int *)(c + 0x370) >= 0x14) {
                        *(int *)(c + 0x370) = 0;
                    }
                    *(char **)(spawned + 0x38c) = c;
                    *(int *)(spawned + 0xa4) = 0;
                    *(int *)(spawned + 0xa8) = 0x5000;
                    *(int *)(spawned + 0xac) = 0;
                }
                *(u16 *)(c + 0x100) = 0x50;
            }
            hit = *(char **)(c + 0x31c);
            if (hit != 0) {
                if (*(int *)(c + 0x30c) == 0) {
                    uid = *(int *)(hit + 4);
#pragma opt_strength_reduction off
                    for (i = 0; i < 0x14; i++) {
                        int slot = *(int *)(c + 0x320 + i * 4);
                        if (slot == uid) {
                            *(int *)(c + 0x374) = i;
                            p30c = (int *)(int)M(c + 0x30c);
                            *p30c = *p30c + 1;
                            func_02012790(0x25);
                            hit = *(char **)(c + 0x31c);
                            posp = (int *)(int)M(hit + 0x5c);
                            npos1.x = posp[0];
                            npos1.y = posp[1];
                            npos1.z = posp[2];
                            _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(
                                c, &npos1, *(unsigned int *)(c + 0x30c), 0, 0, 0);
                            *(char **)(c + 0x31c) = 0;
                            return 1;
                        }
                    }
                } else {
                    p374 = (int *)(int)M(c + 0x374);
                    *p374 = *p374 + 1;
                    if (*(int *)(c + 0x374) >= 0x14) {
                        *(int *)(c + 0x374) = 0;
                    }
                    {
                        uid = *(int *)(*(char **)(c + 0x31c) + 4);
                        i = *(int *)(c + 0x374);
                        while (1) {
                            int slot = *(int *)(c + 0x320 + i * 4);
                            if (slot == uid) {
                                p30c = (int *)(int)M(c + 0x30c);
                                *p30c = *p30c + 1;
                                func_02012790(0x25);
                                hit = *(char **)(c + 0x31c);
                                posp = (int *)(int)M(hit + 0x5c);
                                npos2.x = posp[0];
                                npos2.y = posp[1];
                                npos2.z = posp[2];
                                _ZN5Actor11SpawnNumberERK7Vector3jbtPS_(
                                    c, &npos2, *(unsigned int *)(c + 0x30c), 0, 0, 0);
                                *(char **)(c + 0x31c) = 0;
                                return 1;
                            }
                            break;
                        }
                    }
                }
                *(int *)(c + 0x30c) = 0;
                *(int *)(c + 0x374) = 0;
                *(char **)(c + 0x31c) = 0;
            }
        }
    } else {
        *(int *)(c + 0x304) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x304), 0x7b,
            *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64), 0, 0);
    }

    if (*(int *)(c + 0x30c) == 5) {
        p310 = (int *)(int)M(c + 0x310);
        spos.x = *(int *)(c + 0x5c);
        spos.y = *(int *)(c + 0x60);
        spos.z = *(int *)(c + 0x64);
        spos.y = spos.y - 0x64000;
        *p310 = *p310 + 1;
        if (*(int *)(c + 0x310) > 0x1e) {
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xb2, *(int *)(c + 0x314) | 0x40, &spos,
                (struct Vec3_16 *)(c + 0x8c),
                *(signed char *)(c + 0xcc), -1);
            *(int *)(c + 0x30c) = 0xa;
        }
    }
    return 1;
}
