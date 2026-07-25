extern short data_ov006_0213d6f4;
extern char *data_ov006_02142570;
extern char *data_ov006_02142574;
extern short data_ov006_02142558;
extern short data_ov006_0214255c;

extern void func_ov006_020f9db8(char *c, int b);
extern int func_ov006_020f9bec(char *c);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern int func_ov006_020f9cbc(char *a, char *b);
extern unsigned int func_02012790(unsigned int a);
extern void _Z14ApproachLinearRiii(int *ref, int target, int step);
extern int func_0203d5dc(void *a, void *b);
extern int _Z15ApproachLinear2Rsss(short *ref, short target, short step);

void func_ov006_020f9994(char *c, int b)
{
    switch (*(unsigned char *)(c + 0x2d)) {
    case 0:
        {
            short *p = (short *)(((long long)(int)(c + 0x28)) & 0xffffffffffffffffLL);
            short v = *(short *)p;
            *(short *)p = (short)(v - 1);
        }
        if (*(short *)(c + 0x28) != 0)
            return;
        data_ov006_0213d6f4--;
        func_ov006_020f9db8(c, b);
        return;
    case 2:
        if (*(short *)(c + 0x2a) != b) {
            func_ov006_020f9db8(c, b);
            return;
        }
        if (func_ov006_020f9bec(c) == 0)
            return;
        if (data_ov006_02142570 == 0) {
            data_ov006_02142570 = c;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x153);
            return;
        }
        if (func_ov006_020f9cbc(c, data_ov006_02142570) != 0) {
            data_ov006_02142574 = c;
            data_ov006_02142558 = 0x20;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x154);
            return;
        }
        *(unsigned char *)(data_ov006_02142570 + 0x2d) = 2;
        data_ov006_02142570 = 0;
        func_02012790(0xe);
        return;
    case 3:
        if (func_ov006_020f9bec(c) == 0)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        if (data_ov006_02142570 != c)
            return;
        if (data_ov006_02142574 != 0)
            return;
        _ZN5Sound12PlayBank2_2DEj(0x155);
        data_ov006_02142570 = 0;
        return;
    case 1:
    case 4:
        _Z14ApproachLinearRiii((int *)(c + 0xc), *(int *)(c + 0x14), *(int *)(c + 0x1c));
        _Z14ApproachLinearRiii((int *)(c + 0x10), *(int *)(c + 0x18), *(int *)(c + 0x20));
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0x4000, 0x300);
        if (func_0203d5dc(c + 0xc, c + 0x14) != 0)
            return;
        if (*(int *)(c + 0x24) != 0x4000)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        _Z15ApproachLinear2Rsss(&data_ov006_0214255c, 0, 1);
        *(int *)(c + 0x1c) = 0;
        *(int *)(c + 0x20) = 0;
        return;
    case 5:
        if (_Z15ApproachLinear2Rsss((short *)(c + 0x28), 0, 1) == 0)
            return;
        _Z14ApproachLinearRiii((int *)(c + 0xc), -0x30000, 0x10000);
    }
}
