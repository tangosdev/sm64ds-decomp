typedef struct Node Node;
typedef struct Entry Entry;

struct Node {
    Node *next;
    char pad[4];
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    char pad2[12];
    unsigned short f2c;
    unsigned short f2e;
};

struct Entry {
    char pad0[4];
    short f4;
    unsigned char f6;
    char pad;
    Node *node;
};

extern int _ZN4cstd3divEii(int a, int b);
extern void func_0204dcc0(int *src, Entry *entry, short *value, int *out);

void func_0204dab4(void *sys, Entry *buf, int count, unsigned int mask, void *cb)
{
    int sp[4];
    int i;
    int j;
    Node *walk;
    Entry *slot;
    Entry *p;
    int empty;
    unsigned char *fp6;
    void (*fn)(int *, Entry *, short *, int *);

    if (cb == 0)
        fn = (void (*)(int *, Entry *, short *, int *))func_0204dcc0;
    else
        fn = (void (*)(int *, Entry *, short *, int *))cb;

    walk = *(Node **)((char *)sys + 8);
    if (walk == 0)
        goto tail;

    do {
        slot = 0;
        empty = -1;

        for (i = 0; i < count; i++) {
            Node *n = buf[i].node;

            if (n != 0) {
                if (walk == n) {
                    slot = &buf[i];
                    break;
                }
            } else if (empty == -1) {
                empty = i;
            }
        }

        if (slot == 0 && empty != -1 && mask < 4) {
            slot = &buf[empty];
            slot->node = walk;
            fp6 = (unsigned char *)((int)slot + 6);
            *fp6 = (unsigned char)((*fp6 & ~3u) | (mask & 3u));
        }

        if (slot != 0) {
            int scale;

            scale = 0x1000 -
                _ZN4cstd3divEii((int)walk->f2e << 12, walk->f2c);

            sp[1] = walk->f14 + walk->f8;
            sp[2] = walk->f18 + walk->fc;
            sp[3] = walk->f1c + walk->f10;

            fn(&sp[1], slot, (short *)((char *)slot + 2), &sp[0]);

            slot->f4 = (short)(((long long)sp[0] * scale + 0x800) >> 12);
            fp6 = (unsigned char *)((int)slot + 6);
            *fp6 = (unsigned char)(*fp6 | 4u);
        }

        walk = walk->next;
    } while (walk != 0);

tail:
    j = 0;
    p = buf;

    if (count <= 0)
        return;

    do {
        if ((((unsigned int)p->f6 << 29) >> 31) == 0) {
            if (p->node != 0)
                p->node = 0;
        }

        fp6 = (unsigned char *)((int)p + 6);
        *fp6 = (unsigned char)(*fp6 & ~4u);
        p++;
    } while (++j < count);
}
