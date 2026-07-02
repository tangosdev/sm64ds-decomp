struct Actor;
extern struct Actor *func_0200e55c(unsigned int ownerID);

int func_02008080(void *param_1)
{
    struct Actor *a;
    int *dst;

    a = func_0200e55c(0x13);
    dst = (int *)(((int)param_1 + 0x90) & 0xFFFFFFFFFFFFFFFF);
    *dst = *dst + *(int *)((char *)a + 0x60);
    return 1;
}