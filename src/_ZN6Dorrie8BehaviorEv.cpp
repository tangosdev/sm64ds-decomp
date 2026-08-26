//cpp
#pragma opt_propagation off
#include "Dorrie.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };

extern "C" {
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern int AngleDiff(int a, int b);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void* c, void* p);
extern void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
extern void _ZN9Animation7AdvanceEv(void* p);
extern void func_ov065_02118cc4(char* c);
extern void func_ov065_02118838(char* c);
extern void func_ov065_02118248(char* c);

extern PMF data_ov065_0211d7fc[];
extern short data_02082214[];
}

int Dorrie::Behavior()
{
    char* c = (char*)this;
    int d;
    int a2;
    int thr;

    if (*(unsigned char*)(c + 0x11b5) != 0)
        _Z14ApproachLinearRiii((int*)(c + 0x11ac), 0xa000, 0x1000);
    else
        _Z14ApproachLinearRiii((int*)(c + 0x11ac), 0, 0x1000);

    {
        PMF* m = &data_ov065_0211d7fc[*(unsigned char*)(c + 0x11b4)];
        (((Klass*)c)->**m)();
    }

    *(int*)(c + 0x11a0) = Vec3_HorzDist(c + 0x5c, c + 0x1194);
    *(short*)(c + 0x11a4) = Vec3_HorzAngle(c + 0x5c, c + 0x1194);

    d = (short)AngleDiff(*(short*)(c + 0x11a4), *(short*)(c + 0x8e));

    {
        short sv = data_02082214[((unsigned short)d >> 4) * 2 + 1];
        short cv = data_02082214[(*(unsigned short*)(c + 0x548) >> 4) * 2];
        int sm = sv * 0x190;
        int t = (int)(((long long)sm * cv + 0x800) >> 12);
        if (d < 0x4000) {
            thr = t + 0x5f8000;
            a2 = 0x97c000;
        } else {
            thr = 0x5f8000;
            a2 = t + 0x97c000;
        }
    }

    if (*(int*)(c + 0x11a0) >= a2) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x1194)
            - (int)(((long long)a2 * data_02082214[(*(unsigned short*)(c + 0x11a4) >> 4) * 2] + 0x800) >> 12);
        *(int*)(c + 0x64) = *(int*)(c + 0x119c)
            - (int)(((long long)a2 * data_02082214[(*(unsigned short*)(c + 0x11a4) >> 4) * 2 + 1] + 0x800) >> 12);
    } else if (*(int*)(c + 0x11a0) <= thr) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x1194)
            - (int)(((long long)thr * data_02082214[(*(unsigned short*)(c + 0x11a4) >> 4) * 2] + 0x800) >> 12);
        *(int*)(c + 0x64) = *(int*)(c + 0x119c)
            - (int)(((long long)thr * data_02082214[(*(unsigned short*)(c + 0x11a4) >> 4) * 2 + 1] + 0x800) >> 12);
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(c, 0);
    dBgCh_Actr_UpdateContinuous_Veneer(c + 0xf50);
    *(int*)(c + 0x60) = *(int*)(c + 0x1198) - *(int*)(c + 0x11ac) - *(int*)(c + 0x11a8);
    _ZN9Animation7AdvanceEv(c + 0x13c);
    func_ov065_02118cc4(c);
    func_ov065_02118838(c);
    func_ov065_02118248(c);
    *(unsigned char*)(c + 0x11b5) = 0;
    *(int*)(c + 0x118c) = 0;
    return 1;
}
