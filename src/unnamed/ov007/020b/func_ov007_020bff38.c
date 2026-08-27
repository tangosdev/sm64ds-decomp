extern int func_ov007_020c8de0(int x);
extern int func_ov007_020c8f98(int x);

struct S {
    int a;
    unsigned int flags;
};

void func_ov007_020bff38(int *t)
{
    if (((unsigned int *)t)[1] & 1) {
        if (func_ov007_020c8de0(t[0xe]))
            ((unsigned int *)t)[1] &= ~1u;
    }
    if (((unsigned int *)t)[1] & 2) {
        if (func_ov007_020c8de0(t[0xf]))
            ((unsigned int *)t)[1] &= ~2u;
    }
    if (((unsigned int *)t)[1] & 4) {
        if (func_ov007_020c8f98(t[0x10]))
            ((unsigned int *)t)[1] &= ~4u;
    }
}
