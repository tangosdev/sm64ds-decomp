extern void NullDestructor_0203d47c(void);
extern void func_0207328c(void *a, int b, int c, void *d);

void *func_ov006_020ea324(void *self)
{
    func_0207328c((char *)self + 0x18, 5, 8, (void *)NullDestructor_0203d47c);
    return self;
}
