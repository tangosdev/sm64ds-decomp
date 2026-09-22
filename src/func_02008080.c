struct dActor_c;
extern struct dActor_c *func_0200e55c(unsigned int ownerID);

int func_02008080(void *param_1)
{
    struct dActor_c *a;
    int *dst;

    a = func_0200e55c(0x13);
    dst = (int *)(((int)param_1 + 0x90));
    *dst = *dst + *(int *)((char *)a + 0x60);
    return 1;
}