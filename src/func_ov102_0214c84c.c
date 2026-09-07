#include "types.h"
extern int data_ov102_0214ea48[];

extern void _ZN9dBgCh_GndC1Ev(void *self);
extern void _ZN5dBgCh19StartDetectingWaterEv(void *self);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void *self, void *pos, void *act);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void *self);
extern void _ZN9dBgCh_GndD1Ev(void *self);
extern int SurfaceInfo_TestFlag0x20(int *p);
extern int func_02037e84(int *p);
extern int func_02037e38(unsigned int *p);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir, void *cb);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *pos, u32 e);

void func_ov102_0214c84c(char *c)
{
    int pv[6];
    char rg[0x50];

    pv[0] = *(int *)(c + 0x5c);
    pv[1] = *(int *)(c + 0x60);
    pv[2] = *(int *)(c + 0x64);

    if (*(int **)(c + 0x3ac) == data_ov102_0214ea48
        && *(void **)(c + 0x3c0) != 0
        && *(u8 *)(*(char **)(c + 0x3c0) + 0x6de) == 0)
    {
        _ZN9dBgCh_GndC1Ev(rg);
        _ZN5dBgCh19StartDetectingWaterEv(rg);
        {
            int vx = *(int *)(c + 0x5c);
            int vz = *(int *)(c + 0x64);
            int vy = *(int *)(c + 0x60) + 0x32000;
            pv[3] = vx;
            pv[4] = vy;
            pv[5] = vz;
        }
        _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(rg, &pv[3], c);
        if (_ZN9dBgCh_Gnd10DetectClsnEv(rg))
        {
            if (SurfaceInfo_TestFlag0x20((int *)(rg + 0x14)))
            {
                pv[1] = *(int *)(rg + 0x44) + 0x3c000;
                *(int *)(c + 0x3d4) = 0;
                *(int *)(c + 0x3c8) = *(int *)(c + 0x3d4);
                *(u32 *)(c + 0x3cc) = func_02022d00(*(u32 *)(c + 0x3cc), 0xe2, pv[0], pv[1], pv[2], 0);
                *(u32 *)(c + 0x3d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                    *(u32 *)(c + 0x3d0), 0xe3, pv[0], pv[1], pv[2], 0, 0);
                if (*(int *)(c + 0x3dc) != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                    *(int *)(c + 0x3d8) = 0;
                *(int *)(c + 0x3dc) = func_02037e84((int *)(rg + 0x14)) + 0xf2;
                *(u32 *)(c + 0x3d8) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                    *(u32 *)(c + 0x3d8), 0, *(u32 *)(c + 0x3dc), c + 0x74, 0);
                _ZN9dBgCh_GndD1Ev(rg);
                return;
            }
            if (func_02037e38((unsigned int *)(rg + 0x14)) == 1)
            {
                if (func_02037e84((int *)(rg + 0x14)) == 7)
                {
                    pv[1] = *(int *)(rg + 0x44) + 0x3c000;
                    *(int *)(c + 0x3d4) = 0;
                    *(int *)(c + 0x3c8) = *(int *)(c + 0x3d4);
                    *(u32 *)(c + 0x3cc) = func_02022d00(*(u32 *)(c + 0x3cc), 0xe2, pv[0], pv[1], pv[2], 0);
                    *(u32 *)(c + 0x3d0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        *(u32 *)(c + 0x3d0), 0xe3, pv[0], pv[1], pv[2], 0, 0);
                    if (*(int *)(c + 0x3dc) != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                        *(int *)(c + 0x3d8) = 0;
                    *(int *)(c + 0x3dc) = func_02037e84((int *)(rg + 0x14)) + 0xf2;
                    *(u32 *)(c + 0x3d8) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                        *(u32 *)(c + 0x3d8), 0, *(u32 *)(c + 0x3dc), c + 0x74, 0);
                }
                else
                {
                    pv[1] = *(int *)(rg + 0x44) + 0xa000;
                    *(int *)(c + 0x3d0) = 0;
                    *(int *)(c + 0x3cc) = *(int *)(c + 0x3d0);
                    *(int *)(c + 0x3d4) = *(int *)(c + 0x3cc);
                    *(u32 *)(c + 0x3c8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                        *(u32 *)(c + 0x3c8), 0xe1, pv[0], pv[1], pv[2], 0, 0);
                    if (*(int *)(c + 0x3dc) != 0x102)
                        *(int *)(c + 0x3d8) = 0;
                    *(int *)(c + 0x3dc) = 0x102;
                    *(u32 *)(c + 0x3d8) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                        *(u32 *)(c + 0x3d8), 0, *(u32 *)(c + 0x3dc), c + 0x74, 0);
                }
                _ZN9dBgCh_GndD1Ev(rg);
                return;
            }
            if (*(int *)(c + 0x3dc) != func_02037e84((int *)(rg + 0x14)) + 0xf2)
                *(int *)(c + 0x3d8) = 0;
            *(int *)(c + 0x3dc) = func_02037e84((int *)(rg + 0x14)) + 0xf2;
            *(u32 *)(c + 0x3d8) = _ZN5Sound8PlayLongEjjjRK7Vector3s(
                *(u32 *)(c + 0x3d8), 0, *(u32 *)(c + 0x3dc), c + 0x74, 0);
        }
        else
        {
            *(int *)(c + 0x3d8) = 0;
            *(int *)(c + 0x3dc) = 0;
        }
        _ZN9dBgCh_GndD1Ev(rg);
    }

    *(int *)(c + 0x3d0) = 0;
    *(int *)(c + 0x3cc) = *(int *)(c + 0x3d0);
    *(int *)(c + 0x3c8) = *(int *)(c + 0x3cc);
    pv[1] = pv[1] + 0x1e000;
    *(u32 *)(c + 0x3d4) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32 *)(c + 0x3d4), 0xe0, pv[0], pv[1], pv[2], 0, 0);
}
