extern void MultiStore_Int(int val, void *dst, int size);

void func_ov007_020c78b0(void *c)
{
    volatile int li = 0;
    MultiStore_Int(li, c, 0x14);
}
