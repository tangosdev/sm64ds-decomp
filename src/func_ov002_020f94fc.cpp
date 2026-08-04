//cpp
// @symbol _ZN11SoundObject8BehaviorEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daSoundObj_c.h"
// recovered name: daSoundObj_c_Behavior
/* recovered: renamed to Class_Method */
/* daSoundObj_c::Behavior - recovered from vtable slot identity */
struct C;
typedef int (C::*PMF)(void*);
extern PMF data_ov002_0211110c[];

extern "C" {
void _ZN9ActorBase18MarkForDestructionEv(C* c);
void _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int d, int f, bool g);

int _ZN11SoundObject8BehaviorEv(char* cc)
{
    struct daSoundObj_c *self = (struct daSoundObj_c *)(void *)cc;
    C* c = (C*)cc;
    int sel = *(int*)(cc + 8);
    int r = (c->*data_ov002_0211110c[sel])(cc + 0xdc);
    if (r == 0 && self->unk_0d4 == data_0208e430
        && (self->unk_0dc <= 0xa || data_0209b49c > 0x7f)) {
        goto skip;
    }
    _ZN9ActorBase18MarkForDestructionEv(c);
    if (data_0208e430 != 0x22) {
        _ZN5Sound7PlaySubEjjj5Fix12IiEb(data_0208e430, 0x7f, 0, 0x7f000, 0);
    }
skip:
    if (*(int*)(cc + 8) != 6) {
        if (data_0209b490 < self->unk_0d8) self->unk_0dc = self->unk_0de;
    }
    return 1;
}
}
