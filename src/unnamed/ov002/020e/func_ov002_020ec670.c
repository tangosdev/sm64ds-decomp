#include "types.h"
struct dBgPi;
struct dActor_c;
extern int func_0203567c(int p);
extern int _ZNK5dBgPi9GetClsnIDEv(struct dBgPi* thiz);
extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void func_02123804(struct dActor_c* a, char* self);

void func_ov002_020ec670(char* self, int arg)
{
    struct dBgPi* cr;
    struct dActor_c* actor;
    u16 type;

    cr = (struct dBgPi*)func_0203567c(arg);
    if (_ZNK5dBgPi9GetClsnIDEv(cr) == -1) return;

    actor = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(cr));
    if (actor == 0) return;

    type = *(u16*)((char*)actor + 0xc);
    {
        int t = (int)(type == 0xa1);
        if (t != 0) {
            *(u8*)((char*)actor + 0x3a2) = 1;
            return;
        }
    }
    {
        int t = (int)(type == 0xa4);
        if (t != 0) goto docall;
    }
    {
        int t = (int)(type == 0xa5);
        if (t == 0) return;
    }
docall:
    func_02123804(actor, self);
}
