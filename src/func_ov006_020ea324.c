extern void NullDestructor_0203d47c(void);
extern void __cxa_vec_cleanup(void *a, int b, int c, void *d);

void *func_ov006_020ea324(void *self)
{
    __cxa_vec_cleanup((char *)self + 0x18, 5, 8, (void *)NullDestructor_0203d47c);
    return self;
}
