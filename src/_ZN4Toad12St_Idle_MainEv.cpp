//cpp
// @symbol _ZN4Toad12St_Idle_MainEv
#include "Toad.h"

extern "C" {
extern void *_ZN8dActor_c10FindWithIDEj(u32);
extern int _ZN6Player9StartTalkER7fBase_cb(void *, void *, char);
extern int _ZN7Message11PrepareTalkEv();
}

void Toad::St_Idle_Main()
{
    char *self = (char *)this;
    if ((*(int *)(self + 0xf4) & 0x08000000) == 0)
        return;

    void *actor = _ZN8dActor_c10FindWithIDEj(*(u32 *)(self + 0xf8));
    if (actor == 0)
        return;
    int isPlayer = (*(u16 *)((char *)actor + 0xc) == 0xbf) ? 1 : 0;
    if (!isPlayer)
        return;

    mTalkPlayer = (Player *)actor;
    if (_ZN6Player9StartTalkER7fBase_cb(mTalkPlayer, this, 0) == 0)
        return;

    _ZN7Message11PrepareTalkEv();
    SetState(1);
}
