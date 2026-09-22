extern char data_020a8180[];
extern int data_020a6134[];
extern int func_02057e7c(int *p);
extern int func_02057e84(void *node, int value);
extern void func_02058048(void *self);

void func_02060228(void *fn)
{
    char *base = data_020a8180;
    int v = *(int *)(base + 0x38);
    switch (v) {
    case 0x20:
        v = func_02057e7c((int *)data_020a6134[2]);
        if (v != 0) v -= 1;
        break;
    case 0x21:
        v = func_02057e7c((int *)data_020a6134[2]);
        if ((unsigned int)v < 0x1f) v += 1;
        break;
    }
    func_02057e84(base + 0x3c, v);
    *(void **)(base + 0xd0) = base + 0x3c;
    *(void **)(base + 0x30) = fn;
    {
        unsigned int *fp = (unsigned int *)(base + 0x34);
        *fp |= 8;
    }
    func_02058048(base + 0x3c);
}
