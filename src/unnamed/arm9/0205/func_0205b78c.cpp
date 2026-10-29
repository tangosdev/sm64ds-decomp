//cpp
#include "CP15.h"

extern "C" {

typedef struct Node Node;

struct Slot { struct Node *p18; struct Node *p1c; };

struct Node {
    char pad0[4];
    struct Node *f4;
    char pad8[0x10];
    struct Slot slots[1];
};


void func_0205b78c(struct Node *base, int index, struct Node *arg)
{
    struct Node *r3 = base->slots[index].p18;
    if (r3 != 0) {
        if (arg == r3)
            return;
        if ((struct Node *)&base->slots[index] == r3->slots[0].p18) {
            r3->slots[0].p18 = base->slots[index].p1c;
            CP15::FlushDataCache((unsigned int)base->slots[index].p18, 0x3c);
        } else {
            struct Node *p = r3->slots[0].p18;
            if (p != 0) {
                for (;;) {
                    struct Node *next = p->f4;
                    if ((struct Node *)&base->slots[index] == next)
                        break;
                    p = next;
                    if (next == 0)
                        break;
                }
            }
            p->f4 = (struct Node *)base->slots[index].p1c;
            CP15::FlushDataCache((unsigned int)p, 8);
        }
    }
    {
        struct Node *old = arg->slots[0].p18;
        arg->slots[0].p18 = (struct Node *)&base->slots[index];
        base->slots[index].p1c = (struct Node *)old;
        base->slots[index].p18 = arg;
        CP15::FlushDataCache((unsigned int)base, 0x3c);
        CP15::FlushDataCache((unsigned int)arg, 0x3c);
    }
}
}
