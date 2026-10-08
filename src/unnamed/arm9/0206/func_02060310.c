extern unsigned char data_020a8180;
extern void func_02058048(void *self);

void func_02060310(int a, int b, int c)
{
    if (a == 0xb && c != 0) {
        char *base = (char *)&data_020a8180;
        unsigned int *addr = (unsigned int *)(((int)base + 0x34));
        unsigned int val = *addr;
        val &= ~0x20u;
        *addr = val;
        func_02058048(*(void **)(base + 0xd0));
    }
}
