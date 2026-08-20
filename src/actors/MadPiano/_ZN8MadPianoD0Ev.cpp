//cpp
// @symbol _ZN8MadPianoD0Ev
/* recovered: named members + shared header */
#include "MadPiano.h"
struct Heap;

extern "C" void _ZN10dBgCh_ActrD1Ev(void *thiz);
extern "C" void __destroy_arr(void *p, int a, int b, void *fn);
extern "C" void _ZN11ShadowModelD1Ev(void *thiz);
extern "C" void _ZN9ModelAnimD1Ev(void *thiz);
extern "C" void _ZN10dBgW_KcMbgD1Ev(void *thiz);
extern "C" void _ZN5ModelD1Ev(void *thiz);
extern "C" void _ZN8dActor_cD2Ev(void *thiz);
extern "C" void _ZN6Memory10DeallocateEPvP4Heap(void *p, struct Heap *h);
extern "C" void _ZN10dCcAcPos_cD1Ev(void *thiz);
extern void *_ZTV8MadPiano[];
extern void *_ZTV10dBgActor_c[];
extern struct Heap *data_020a0eac;

extern "C" void *_ZN8MadPianoD0Ev(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    *(void **)c = _ZTV8MadPiano;
    _ZN10dBgCh_ActrD1Ev(c + 0x50c);
    __destroy_arr(c + 0x48c, 2, 0x40, (void *)_ZN10dCcAcPos_cD1Ev);
    _ZN11ShadowModelD1Ev(c + 0x3d4);
    _ZN11ShadowModelD1Ev(c + 0x3ac);
    _ZN11ShadowModelD1Ev(c + 0x384);
    _ZN9ModelAnimD1Ev(c + 0x320);
    *(void **)c = _ZTV10dBgActor_c;
    _ZN10dBgW_KcMbgD1Ev(c + 0x124);
    _ZN5ModelD1Ev(c + 0xd4);
    _ZN8dActor_cD2Ev(c);
    _ZN6Memory10DeallocateEPvP4Heap(c, data_020a0eac);
    return c;
}
