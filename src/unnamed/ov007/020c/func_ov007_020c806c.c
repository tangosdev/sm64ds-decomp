extern void MultiStore_Int(int val, void *dst, int size);

void func_ov007_020c806c(void *c)
{
    volatile int li = 0;
    MultiStore_Int(li, c, 0x18);
}
