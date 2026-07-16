//cpp
extern "C" {
extern unsigned char data_0209f250;
extern signed char data_02092110;
extern int data_020873dc;
extern int data_0208742c;
extern int data_0209b0e8;
extern void func_0200cb58(void *obj, int index);
extern void func_0200c66c(void *self, void *base, int *a, int *b, int *c);
extern void _ZN6Camera11ChangeStateEPNS_5StateE(void *self, void *st);

void _ZN6Camera14GoBehindPlayerEj(int self, unsigned int j)
{
    int slot4, slot8, slotc;

    if (j != data_0209f250)
        return;
    if (data_02092110 >= 0)
        return;

    /* different launder spellings to defeat CSE across the call */
    *(unsigned int *)((self + 0x154) & 0xFFFFFFFFFFFFFFFF) &= 0xfffffaf7u;
    func_0200cb58((void *)self, 0xa);
    *(unsigned int *)(((long long)(int)(self + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) |= 4u;

    slot4 = *(int *)(self + 0x13c);
    slotc = 0;
    func_0200c66c((void *)self, (void *)(*(int *)(self + 0x110) + 0x5c), &slot8, &slot4, &slotc);
    if (slot4 == (int)&data_020873dc)
        return;
    if (slot4 == (int)&data_0208742c)
        return;
    _ZN6Camera11ChangeStateEPNS_5StateE((void *)self, &data_0209b0e8);
}
}
