extern void func_ov004_020b9280(void);
extern void __destroy_arr(void *a, int b, int c, void *d);

void *func_ov004_020b929c(void *self)
{
    __destroy_arr(self, 8, 0x24, (void *)func_ov004_020b9280);
    return self;
}
