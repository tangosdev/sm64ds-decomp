#include "common.h"

extern void func_0201267c(int a, char *b);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(int a, int b, int x, int y, int z, int v, int cb);
extern void _ZN8Particle19SetSelfDestructFlagEj(int id);
extern char *_ZN8Particle6System12FromUniqueIDEj(int id);
extern s16 data_02082214[];

// 6f: keep constant live / flip coloring
#pragma opt_propagation off
void func_ov062_02117724(char *t, unsigned int a1, unsigned int a2, unsigned int a3, unsigned short a4) /* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    unsigned int h = (unsigned int)(*(int *)(t + 0x358) << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        char *ps;
        if (*(unsigned char *)(t + 0x3cd) != 0) return;
        *(unsigned char *)(t + 0x3cd) = 1;
        func_0201267c(0xe4, t + 0x74);
        if (*(unsigned char *)(t + 0x398) == 1) return;
        ang = (short)(*(short *)(t + 0x8e) + 0x4000);
        pos.x = *(int *)(t + 0x5c);
        pos.y = *(int *)(t + 0x60);
        pos.z = *(int *)(t + 0x64);
        iscb = *(unsigned short *)(t + 0xc) == 0xcb;
        k = iscb ? 0xf : 0xa;
        *(volatile int *)&pos.y = pos.y + (k << 12); /* pin y add dest */
        if (h <= a2) {
            int i2, i1, v, w;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            v = data_02082214[i2];
            v = pos.x - k * v;
            w = data_02082214[i1];
            w = pos.z - k * w;
            pos.x = v;
            pos.z = w;
        } else {
            int i2, i1, ev, ew;
            idx = (unsigned short)ang >> 4;
            i2 = idx * 2;
            i1 = i2 + 1;
            ev = k * data_02082214[i2] + pos.x;
            ew = k * data_02082214[i1] + pos.z;
            pos.x = ev;
            pos.z = ew;
        }
        uid = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(0, 0xf9, pos.x, pos.y, pos.z, 0, 0);
        _ZN8Particle19SetSelfDestructFlagEj(0xf9);
        if (uid == 0) return;
        ps = _ZN8Particle6System12FromUniqueIDEj(uid);
        if (ps == 0) return;
        iscb2 = *(unsigned short *)(t + 0xc) == 0xcb;
        if (iscb2)
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x800 + 0x800) >> 12);
        else
            *(int *)(ps + 0x50) = (short)(((long long)*(int *)(ps + 0x50) * 0x500 + 0x800) >> 12);
    } else {
        *(unsigned char *)(t + 0x3cd) = 0;
    }
}
