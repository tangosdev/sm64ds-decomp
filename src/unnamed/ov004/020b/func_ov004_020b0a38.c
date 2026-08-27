extern int *data_ov004_020beb68[];

void func_ov004_020b0a38(void)
{
    int *p = (int *)data_ov004_020beb68[0];
    if (p == 0) return;
    *(int *)((char *)p + 0xf0) = 0;
}
