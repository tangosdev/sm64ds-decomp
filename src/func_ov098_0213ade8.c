#include "decl_common.h"

extern s16 data_02082214[];
extern Vector3_16 data_ov098_0213c63c;

extern int _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_Dist(void* a, void* b);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* pos);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* pos, void* rot, int area, int death);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* c);
extern void _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int slot, unsigned int unk, Fix12i x, Fix12i y, Fix12i z, void* rot, void* cb);

void func_ov098_0213ade8(char* self)
{
    Vector3_16 rot;
    Vector3 pos;
    volatile int vel[3];
    int state;
    int closest;

    if (*(int*)(self + 0x174) != 0) {
        int *p = (int*)(int)(self + 0x174);
        *p = *p - 1;
    }

    closest = _ZN5Actor13ClosestPlayerEv(self);
    if (Vec3_Dist(self + 0x5c, (char*)closest + 0x5c) >= 0x800000) {
        return;
    }

    state = *(int*)(self + 0x174);
    if (state == 0) {
        *(int*)(self + 0x180) = 0;
        func_ov098_0213b15c(self);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x14c, self + 0x74);
        return;
    }

    if (state == 0x3c) {
        char* a;
        s16 *tbl;
        int k;
        s16 ang;
        int idx;

        pos.x = *(int*)(self + 0x5c);
        pos.y = *(int*)(self + 0x60);
        pos.z = *(int*)(self + 0x64);
        pos.y = pos.y + 0x80000;

        {
            void *sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0xd0, 3, &pos, 0, *(signed char*)(self + 0xcc), -1);
            s16 *t = data_02082214;
            int kk = 0x64;
            s16 pitch = *(s16*)(self + 0x92);
            int tval = (s16)(0x4000 - pitch);
            int i = (u16)tval >> 4;
            int s = t[i * 2];
            tbl = t;
            k = kk;
            a = (char*)sp;
            *(int*)(a + 0xa4) = 0;
            *(int*)(a + 0xa8) = s * kk;
            *(int*)(a + 0xac) = 0;
        }

        ang = (s16)(0x4000 - *(s16*)(self + 0x92));
        idx = (u16)ang >> 4;
        *(int*)(a + 0x98) = tbl[idx * 2 + 1] * k;

        {
            s16 yaw = *(s16*)(self + 0x94);
            *(s16*)(a + 0x92) = 0;
            *(s16*)(a + 0x94) = yaw;
            *(s16*)(a + 0x96) = 0;
        }

        _ZN5Actor9UpdatePosEP12CylinderClsn(a, 0);

        {
            int *vp = (int*)(int)(a + 0xa4);
            int vx = *vp;
            int px = pos.x;
            vel[0] = vx;
            int vy = *(vp + 1);
            int py = pos.y;
            vel[1] = vy;
            int vz = *(vp + 2);
            int x = px + (vx << 1);
            int pz = pos.z;
            vel[2] = vz;
            int y = py + (vy << 1);
            *(int*)(self + 0x168) = x;
            *(int*)(self + 0x16c) = y;
            *(int*)(self + 0x170) = pz + (vz << 1);
        }

        rot = data_ov098_0213c63c;
        rot.x = (u16)data_02082214[(*(u16*)(self + 0x94) >> 4) * 2];
        rot.z = (u16)data_02082214[(*(u16*)(self + 0x94) >> 4) * 2 + 1];

        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 7, *(int*)(self + 0x168), *(int*)(self + 0x16c), *(int*)(self + 0x170),
            &rot, 0);
        _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            0, 8, *(int*)(self + 0x168), *(int*)(self + 0x16c), *(int*)(self + 0x170),
            &rot, 0);

        _ZN5Sound9PlayBank3EjRK7Vector3(0xd4, self + 0x74);
        *(int*)(self + 0x18c) = -0x1800;
        return;
    }

    {
        int *p188 = (int*)(int)(self + 0x188);
        int *p18c = (int*)(int)(self + 0x18c);
        *p188 = *p188 + *(int*)(self + 0x18c);
        *p18c = *p18c + 0xc00;
    }
    if (*(int*)(self + 0x18c) >= 0x800) {
        *(int*)(self + 0x18c) = 0x800;
    }
    if (*(int*)(self + 0x188) >= 0) {
        *(int*)(self + 0x18c) = 0;
        *(int*)(self + 0x188) = *(int*)(self + 0x18c);
    }
}
