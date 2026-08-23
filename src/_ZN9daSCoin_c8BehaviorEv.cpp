//cpp
// @symbol _ZN9daSCoin_c8BehaviorEv
/* daSCoin_c::Behavior -- vtable slot 6. Real C++ method over the shared header,
 * named members and real inherited method calls. */
#include "decl_common.h"
#include "daSCoin_c.h"
extern "C" unsigned char DecIfAbove0_Byte(unsigned char* p);

s32 daSCoin_c::Behavior()
{
    char *c = (char *)this;
    daSCoin_c *self = this;
    if (self->unk_113) {
        if (DecIfAbove0_Byte((unsigned char *)(c + 0x113)) == 0) {
            func_ov002_020f05f4(c);
            ((fBase_c *)c)->MarkForDestruction();
        }
        return 1;
    }
    if (self->unk_10f == 0) {
        unsigned char st = self->unk_10e;
        if (st == 0 || st == 0xf) {
            {
                dActor_c *o = 0;
                self->unk_10f = 1;
                self->unk_108 = *(int *)(c + 4);
                for (;;) {
                    o = dActor_c::FindWithActorID(0x149, o);
                    if (o == 0) break;
                    if (o != (dActor_c *)c) {
                        *(unsigned char *)((char *)o + 0x10f) = 2;
                        *(int *)((char *)o + 0x108) = *(int *)(c + 4);
                    }
                }
            }
        }
    }
    if (self->unk_10f == 1 && self->unk_110 == 5) {
        ((fBase_c *)c)->MarkForDestruction();
        return 1;
    }
    func_ov002_020f051c(c);
    if (*(int *)(c + 0xf8)) {
        func_ov002_020f0438(c);
    }
    ((dCc_c *)(c + 0xd4))->Clear();
    if (self->unk_111 == 0) {
        ((dCc_c *)(c + 0xd4))->Update();
    }
    return 1;
}
