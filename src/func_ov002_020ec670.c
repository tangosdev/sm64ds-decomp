#include "types.h"
struct ClsnResult;
struct Actor;
extern int func_0203567c(int p);
extern int _ZNK10ClsnResult9GetClsnIDEv(struct ClsnResult* thiz);
extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_02123804(struct Actor* a, char* self);

void func_ov002_020ec670(char* self, int arg)
{
    struct ClsnResult* cr;
    struct Actor* actor;
    u16 type;

    cr = (struct ClsnResult*)func_0203567c(arg);
    if (_ZNK10ClsnResult9GetClsnIDEv(cr) == -1) return;

    actor = _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(cr));
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
