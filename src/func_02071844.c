extern int data_020737a8;
extern void __sinit_02073a24(void);
int func_02071844(void **p)
{
    p[3] = &data_020737a8;
    p[4] = (void *)__sinit_02073a24;
    return 1;
}
