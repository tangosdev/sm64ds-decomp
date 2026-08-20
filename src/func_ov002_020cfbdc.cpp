//cpp
// @symbol func_ov002_020cfbdc
/* recovered: shared common types, declarations from a shared header */
#include "decl_dBgPi.h"
/* recovered: shared common types */
#include "common.h"


extern "C" {
    extern void func_ov002_020d0948(void*);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
extern void *_ZNK10dBgCh_Actr14GetFloorResultEv(void *self);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN9dBgCh_LinC1Ev(void *self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void *self, void *a, void *b, void *act);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void *self);
extern void _ZN9dBgCh_LinD1Ev(void *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(void *self, void *st);
extern short data_02082214[];
extern int data_ov002_021101b4;
}

extern "C" int func_ov002_020cfbdc(char *self)
{
    Vector3 pts[3];
    char rl[0x78];
    int idx = (unsigned short)(short)(*(short *)(self + 0x8e) + 0x8000) >> 4;
    int r6 = data_02082214[idx * 2];
    int r5 = data_02082214[idx * 2 + 1];

    if (*(unsigned short *)(self + 0x6a6) == 0) {
        if ((*(unsigned char *)(self + 0x6e9) & 2) != 0) {
            if (_ZNK10dBgCh_Actr10IsOnGroundEv(self + 0x380) != 0) {
                void *fr = _ZNK10dBgCh_Actr14GetFloorResultEv(self + 0x380);
                if (_ZNK5dBgPi9GetClsnIDEv(fr) != -1) {
                    if (_ZN8dActor_c10FindWithIDEj((unsigned int)_ZNK5dBgPi9GetClsnIDEv(fr)) != 0) {
                        func_ov002_020d0948(self);
                        return 1;
                    }
                }
            }
        }
    }

    {
        int az = *(int *)(self + 0x64) + (r5 << 5);
        int ay = *(int *)(self + 0x60) - 0xa000;
        int ax = *(int *)(self + 0x5c) + (r6 << 5);
        pts[1].x = ax;
        pts[1].y = ay;
        pts[1].z = az;
    }
    {
        int bx = *(int *)(self + 0x5c) + r6 * 0x60;
        int bz = *(int *)(self + 0x64) + r5 * 0x60;
        int by = *(int *)(self + 0x60) - 0xa000;
        pts[2].x = bx;
        pts[2].y = by;
        pts[2].z = bz;
    }

    _ZN9dBgCh_LinC1Ev(rl);
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &pts[1], &pts[2], self);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl) != 0) {
        *(int *)((int)(self + 0x5c)) += r5 * 0x60;
        *(int *)((int)(self + 0x60)) -= 0x80000;
        *(int *)((int)(self + 0x64)) += r5 * 0x60;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021101b4);
        _ZN9dBgCh_LinD1Ev(rl);
        return 1;
    }
    _ZN9dBgCh_LinD1Ev(rl);
    return 0;
}
