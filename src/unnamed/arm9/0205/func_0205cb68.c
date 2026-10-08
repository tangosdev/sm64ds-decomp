extern void func_0205d2dc(void);
extern void func_0205d2b0(void);

int func_0205cb68(int *self, int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    self[7] = a1;    /* 0x1c */
    self[9] = a3;    /* 0x24 */
    self[0xc] = a2;  /* 0x30 */
    self[8] = self[0xc];   /* 0x20 = self[0x30] */
    self[0xb] = a5;  /* 0x2c */
    self[0xd] = a4;  /* 0x34 */
    self[0xa] = self[0xd]; /* 0x28 = self[0x34] */
    self[0xf] = a6 ? a6 : (int)func_0205d2dc;   /* 0x3c */
    self[0x10] = a7 ? a7 : (int)func_0205d2b0;  /* 0x40 */
    self[0x11] = self[0xf]; /* 0x44 = self[0x3c] */
    self[0xe] = 0;   /* 0x38 */
    {
        int *pp = (int *)(((int)self + 0x10));
        *pp |= 2;    /* 0x10 */
    }
    return 1;
}
