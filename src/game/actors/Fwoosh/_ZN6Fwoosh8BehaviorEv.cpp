//cpp
// @symbol _ZN6Fwoosh8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Fwoosh.h"
struct dBgCh_Actr;
struct dCc_c;
struct dEnemyBase_c;
typedef void (dEnemyBase_c::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
extern int _ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(dEnemyBase_c *thiz, dBgCh_Actr *c);
extern void _ZN5dCc_c5ClearEv(void *thiz);
extern void _ZN5dCc_c6UpdateEv(void *thiz);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(dEnemyBase_c *thiz, void *clsn);
extern void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(dEnemyBase_c *thiz, dBgCh_Actr *wm, unsigned int j);
extern void _ZN9Animation7AdvanceEv(void *thiz);
}


int Fwoosh::Behavior()
{
    char *c = (char *)((dEnemyBase_c *)this);
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((dEnemyBase_c *)this), (dBgCh_Actr *)(c + 0x144)) != 0) {
        _ZN5dCc_c5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN5dCc_c6UpdateEv(c + 0x110);
            }
        }
        func_ov091_02134094(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        Holder *q = *(Holder **)(c + 0x364);
        if (q->fn != 0) (((dEnemyBase_c *)this)->*(q->fn))();
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    {
        int v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int hi = *(int *)(c + 0xa0);
        if (v >= hi)
            hi = v;
        int tmp = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = hi;
        *(int *)(c + 0xac) = tmp;
    }
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(((dEnemyBase_c *)this), (void *)(c + 0x110));
    func_ov091_021339fc(c);
    _ZN5dCc_c5ClearEv(c + 0x110);
    _ZN5dCc_c6UpdateEv(c + 0x110);

    if (*(int *)(c + 0x374) == 1) {
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((dEnemyBase_c *)this), (dBgCh_Actr *)(c + 0x144), 0);
        return 1;
    }

    _ZN9Animation7AdvanceEv(c + 0x350);
    func_ov091_02134094(c);
    return 1;
}
