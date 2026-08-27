extern int func_02058764(void);
extern int data_020a63a8;
extern int data_020a63a0;
extern void func_020593f4(void);

void func_02059594(void)
{
    unsigned int v = *(unsigned int *)0x27ffd9c;
    if (v >= 0x2600000 && v < 0x2800000)
        data_020a63a8 = v;
    else
        data_020a63a8 = 0;
    if (data_020a63a8 == 0 || (func_02058764() & 0x40000000) == 0) {
        char *base = (char *)0x23c3000;
        *(void **)0x27ffd9c = (void *)func_020593f4;
        *(void **)(base + 0xfdc) = (void *)func_020593f4;
    }
    data_020a63a0 = 0;
}
