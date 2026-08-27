#include "Unagi.h"
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN12dEnemyBase_cC2Ev(void *);
extern void _ZN10dCcAcPos_cC1Ev(void *);
extern void _ZN10dBgCh_ActrC1Ev(void *);
extern void _ZN14BlendModelAnimC1Ev(void *);
extern void func_020733a8(void*, int, int, void*, void*);
extern int _ZTV5Unagi[];
extern int _ZN7Vector3D1Ev[];
extern int func_0203d384[];
int *Unagi_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(sizeof(struct Unagi));
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV5Unagi;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x110);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x150);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x190);
        _ZN14BlendModelAnimC1Ev((char *)p + 0x350);
        func_020733a8((char *)p + 0x448, 7, 0xc, (void*)func_0203d384, (void*)_ZN7Vector3D1Ev);
    }
    return p;
}
