struct Node;

struct Slot { struct Node *p18; struct Node *p1c; };

struct Node {
    char pad0[4];
    struct Node *f4;
    char pad8[0x10];
    struct Slot slots[1];
};

extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

void func_0205b78c(struct Node *base, int index, struct Node *arg)
{
    struct Node *r3 = base->slots[index].p18;
    if (r3 == 0) goto L90;
    if (arg == r3) return;
    if ((struct Node *)&base->slots[index] == r3->slots[0].p18) {
        r3->slots[0].p18 = base->slots[index].p1c;
        _ZN4CP1514FlushDataCacheEjj((unsigned int)base->slots[index].p18, 0x3c);
        goto L90;
    }
    {
        struct Node *p = r3->slots[0].p18;
        if (p != 0) {
            for (;;) {
                struct Node *next = p->f4;
                if ((struct Node *)&base->slots[index] == next) break;
                p = next;
                if (next == 0) break;
            }
        }
        p->f4 = (struct Node *)base->slots[index].p1c;
        _ZN4CP1514FlushDataCacheEjj((unsigned int)p, 8);
    }
L90:
    {
        struct Node *old = arg->slots[0].p18;
        arg->slots[0].p18 = (struct Node *)&base->slots[index];
        base->slots[index].p1c = (struct Node *)old;
        base->slots[index].p18 = arg;
        _ZN4CP1514FlushDataCacheEjj((unsigned int)base, 0x3c);
        _ZN4CP1514FlushDataCacheEjj((unsigned int)arg, 0x3c);
    }
}
