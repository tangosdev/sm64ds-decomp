//cpp
// @symbol _ZN7Clipper13Func_020156DCEv
/* recovered: named members + shared header */
#include "Clipper.h"
typedef int Fix12i;
typedef unsigned int u32;
typedef unsigned short u16;

extern "C" {

void _ZN7Clipper13Func_0201559CEv(void* self);

void _ZN7Clipper13Func_020156DCEv(struct Clipper *self, u32 a, u16 b, Fix12i c, Fix12i d) {
    self->unk_04c = a;
    self->unk_058 = b;
    *(Fix12i*)((char*)&self->unk_050) = c;
    *(Fix12i*)((char*)&self->unk_054) = d;
    _ZN7Clipper13Func_0201559CEv(((char*)self));
}

}
