extern void _ZN3G2x18SetBlendBrightnessEPVtts(volatile unsigned short*, unsigned short, short);
extern void func_ov007_020b65e0(void);
extern void func_ov007_020b3050(void);
extern void func_ov007_020b7764(void);
extern void func_ov007_020bdf60(void);
extern void func_ov007_020b8690(void);
extern void func_ov007_020bfc54(void);
extern void func_ov007_020be47c(void);
extern void func_ov007_020b40c0(void);

struct S {
    char pad0[0x10];
    int field_10;
    int field_14;
    char pad[0xd8];
    int field_f0;
    char pad2[8];
    unsigned char field_fc;
    unsigned char field_fd;
};

extern struct S* data_ov007_0210342c;
extern int data_ov007_02103430;

int func_ov007_020b6f4c(void)
{
    struct S* p = data_ov007_0210342c;
    int r;
    r = p->field_f0;
    {
        unsigned int t = (p->field_fc << 1) | (p->field_fd << 3);
        data_ov007_02103430 = 0;
        r |= t;
    }
    if (p->field_10 != 0xa && p->field_10 != 0xb && p->field_14 != 0) {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4000050, 0x3f, 0x10);
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x3f, 0x10);
        *(unsigned int*)0x4000000 &= ~0x1f00;
        *(unsigned int*)0x4001000 &= ~0x1f00;
    } else {
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4000050, 0x20, -0x10);
        _ZN3G2x18SetBlendBrightnessEPVtts((volatile unsigned short*)0x4001050, 0x20, -0x10);
    }
    func_ov007_020b65e0();
    func_ov007_020b3050();
    func_ov007_020b7764();
    func_ov007_020bdf60();
    func_ov007_020b8690();
    func_ov007_020bfc54();
    func_ov007_020be47c();
    func_ov007_020b40c0();
    return r;
}
