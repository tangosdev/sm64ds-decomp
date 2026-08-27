extern unsigned char data_0209d454;
extern void func_ov002_020f2f64(void *p);
extern void func_ov002_020f2a48(char *p);

void func_ov002_020f5b24(char *thiz)
{
    data_0209d454 &= ~6;
    data_0209d454 |= 8;
    func_ov002_020f2f64(thiz);
    *(volatile unsigned int *)0x4001000 &= ~0xe000;
    thiz[0x15c] = 0;
    func_ov002_020f2a48(thiz);
}
