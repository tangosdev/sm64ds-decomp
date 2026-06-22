typedef unsigned int u32;
typedef unsigned char u8;
typedef int Fix12i;

struct Node {
    int f0;          /* 0x00 value */
    u8 f4;           /* 0x04 */
    char pad5[3];
    struct Node *f8; /* 0x08 */
    struct Node *fc; /* 0x0c */
};

struct Elem {
    char pad0[8];
    struct Node *f8; /* 0x08 head */
};

extern char *data_0209f314;  /* holds pointer to element array base */
extern void *_Znwj(u32 sz);

void _Z23LoadMinimapChangeObjecti5Fix12IiEh(int idx, Fix12i val, u8 h)
{
    struct Node *nn = (struct Node *)_Znwj(0x10);
    struct Elem *e = (struct Elem *)(data_0209f314 + idx * 0xc);
    struct Node *cur = e->f8;
    struct Node *prev = cur;
    if (cur != 0) {
        for (;;) {
            prev = cur;
            if (val > cur->f0) break;
            cur = cur->fc;
            if (cur == 0) break;
        }
    }
    nn->fc = prev;
    if (prev == 0) {
        nn->f8 = 0;
    } else {
        nn->f8 = prev->f8;
        prev->f8 = nn;
    }
    if (nn->f8 == 0)
        e->f8 = nn;
    {
        int hh = h;
        nn->f0 = val;
        if (hh == 0xf)
            hh = -1;
        nn->f4 = (u8)hh;
    }
}
