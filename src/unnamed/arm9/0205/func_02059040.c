extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int);
extern int data_020a637c[];

struct Blk {
    int f0;
    int f4;
    int f8;
    int fc;
    int *f10;
};

int func_02059040(int idx, struct Blk *b, unsigned int c, int d)
{
    unsigned int saved = (unsigned int)_ZN3IRQ7DisableEv();
    int i;
    int m = d * 0xc;
    data_020a637c[idx] = (int)b;
    b->f10 = (int*)((char*)b + 0x14);
    b->f4 = d;
    for (i = 0; i < b->f4; i++) {
        int *ent = (int*)((char*)b->f10 + i * 0xc);
        ent[0] = -1;
        ent[2] = 0;
        ent[1] = ent[2];
    }
    b->f0 = -1;
    b->f8 = (int)(((char*)b->f10 + m + 0x1f) - 0) & ~0x1f;
    b->fc = c & ~0x1f;
    _ZN3IRQ7RestoreEj(saved);
    return b->f8;
}
