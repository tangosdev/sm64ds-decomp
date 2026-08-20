//cpp
#include "types.h"
// @symbol _ZN10CheepCheep8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CheepCheep.h"
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
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(dEnemyBase_c *thiz, void *clsn);
extern void _ZN9Animation7AdvanceEv(void *thiz);
extern char *_ZN8dActor_c13ClosestPlayerEv(dEnemyBase_c *thiz);
}


int CheepCheep::Behavior()
{
    char *c = (char *)((dEnemyBase_c *)this);
    if (_ZN12dEnemyBase_c14UpdateYoshiEatER10dBgCh_Actr(((dEnemyBase_c *)this), (dBgCh_Actr *)(c + 0x150)) != 0) {
        _ZN5dCc_c5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN5dCc_c6UpdateEv(c + 0x110);
            }
        }
        func_ov090_02133338(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    _ZN8dActor_c9UpdatePosEP5dCc_c(((dEnemyBase_c *)this), (void *)(c + 0x110));
    {
        Holder *q = *(Holder **)(c + 0x370);
        if (q->fn != 0) (((dEnemyBase_c *)this)->*(q->fn))();
    }
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(int *)(c + 0x368) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x35c);
    func_ov090_02133338(c);
    func_ov090_021330c8(c);
    _ZN5dCc_c5ClearEv(c + 0x110);
    {
        char *p = _ZN8dActor_c13ClosestPlayerEv(((dEnemyBase_c *)this));
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN5dCc_c6UpdateEv(c + 0x110);
        }
    }
    return 1;
}
