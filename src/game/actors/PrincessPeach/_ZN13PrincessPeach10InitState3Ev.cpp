//cpp
#include "PrincessPeach.h"

extern "C" {
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int _ZN6Player9StartTalkER7fBase_cb(void *self, void *actor, bool b);
}

int PrincessPeach::InitState3()
{
    char *c = (char *)this;
    if (*(int*)(c+0x180) & 0x8000000) {
        char *a = (char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c+0x184));
        if (a) {
            int match = (*(unsigned short*)(a+0xc) == 0xbf) ? 1 : 0;
            if (match != 0) {
                *(void**)(c+0x35c) = a;
                if (_ZN6Player9StartTalkER7fBase_cb(*(void**)(c+0x35c), c, false)) {
                    SetState(1);
                }
            }
        }
    }
    return 1;
}
