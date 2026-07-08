extern void _ZN9ActorBaseC1Ev(void *self);
extern void func_ov004_020b8a70(unsigned char *p);
extern char *func_ov004_020b92c4(char *c);
extern int func_ov004_020adc3c(void *c);
extern int func_02013580(int a, int b);
extern void func_ov004_020adc00(int v);

extern char _ZTV12ActorDerived;
extern char _ZTV5Scene;
extern char data_ov004_020bc0c0;
extern char data_0209b308;
extern void *data_ov004_020beb68;
extern short data_ov004_020bc070[];

void *func_ov004_020b2adc(char *self)
{
    _ZN9ActorBaseC1Ev(self);
    *(void **)self = &_ZTV12ActorDerived;
    *(void **)self = &_ZTV5Scene;
    {
        unsigned char *p13 = (unsigned char *)(((long long)(int)(self + 0x13)) & 0xFFFFFFFFFFFFFFFFLL);
        *p13 |= 1;
        *p13 |= 4;
    }
    *(void **)self = &data_ov004_020bc0c0;
    *(int *)(self + 0xa4) = 0;
    *(unsigned char *)(self + 0xc2) = 1;

    func_ov004_020b8a70((unsigned char *)(self + 0xcc));
    func_ov004_020b92c4(self + 0xf4);

    *(int *)(self + 0x4628) = 0;
    *(int *)(self + 0x462c) = 0;
    *(int *)(self + 0x4630) = 0;
    *(int *)(self + 0x4648) = 0;

    data_ov004_020beb68 = self;
    *(int *)(self + 0xbc) = *(int *)(&data_0209b308 + 0x30);

    *(short *)(self + 0x465e) = data_ov004_020bc070[(*(unsigned int *)(self + 8) >> 0x10) & 0xff];
    *(int *)(self + 8) = *(unsigned int *)(self + 8) & 0xffff;
    *(int *)(self + 0x50) = 0;
    *(int *)(self + 0x54) = 0;

    {
        unsigned short id = *(unsigned short *)(self + 0xc);
        if (id == 0x16e || id == 0x185 || id == 0x16d || id == 0x182)
            *(int *)(self + 0x58) = 0x3c;
        else
            *(int *)(self + 0x58) = 0x78;
    }

    *(int *)(self + 0x60) = 0;
    *(int *)(self + 0x5c) = 0;
    *(int *)(self + 0x464c) = 0;
    *(int *)(self + 0x4654) = 0;
    *(int *)(self + 0x4658) = 0;
    *(int *)(self + 0x64) = -1;

    func_ov004_020adc00(func_02013580(func_ov004_020adc3c(self), 0));
    return self;
}
