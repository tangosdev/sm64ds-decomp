//cpp
// @symbol _ZN9KoopaFlag8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_Timer.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "KoopaFlag.h"
extern "C" {
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, int d, int e);
extern void _ZN9Animation7AdvanceEv(void *a);
extern void _ZN12CylinderClsn5ClearEv(void *c);
extern void _ZN12CylinderClsn6UpdateEv(void *c);
}
extern char data_0209d4c8[];

int KoopaFlag::Behavior()
{
    unsigned int id;
    char *a;
    int b;

    if (unk_16e == 0) {
        id = unk_0f8;
        if (id != 0) {
            a = _ZN5Actor10FindWithIDEj(id);
            if (a != 0) {
                b = *(unsigned short *)(a + 0xC);
                b = b == 0xBF;
                if (b) {
                    unk_16e = 1;
                    _ZN5Timer9StopTimerEv(data_0209d4c8);
                    mVictoryTimer = 1;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (mVictoryTimer != 0) {
        *(unsigned short *)(((int)((char *)this) + 0x16C)) += 1;
        if (mVictoryTimer >= 0x5A) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x7F, 0, 0x8777, 0) != 0) {
                mVictoryTimer = 0;
            }
        }
    }

    _ZN9Animation7AdvanceEv((char *)&mAnimation);
    func_ov062_0211afbc(((char *)this));
    _ZN12CylinderClsn5ClearEv((char *)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char *)&mMovingCylinderClsn);
    return 1;
}
