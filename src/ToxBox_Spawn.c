#include "ToxBox.h"
extern void* _ZN7fBase_cnwEj(unsigned int size);
extern void _ZN10dBgActor_cC2Ev(void *p);
extern void _ZN10dBgCh_ActrC1Ev(void *p);
extern void _ZN10dCcAcPos_cC1Ev(void *p);
extern void _ZN7PathPtrC1Ev(void *p);
extern int _ZTV6ToxBox[];

int* ToxBox_Spawn(void) {
    int *p = (int*)_ZN7fBase_cnwEj(sizeof(struct ToxBox));
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        *(int**)p = _ZTV6ToxBox;
        _ZN10dBgCh_ActrC1Ev((char*)p + 0x324);
        _ZN10dCcAcPos_cC1Ev((char*)p + 0x4e8);
        _ZN7PathPtrC1Ev((char*)p + 0x58c);
    }
    return p;
}
