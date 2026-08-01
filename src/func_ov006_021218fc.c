#include "types.h"
extern int data_ov006_02140588;
extern int data_ov006_0213b0ec;
extern int data_ov006_0214058c;
extern int data_ov006_02142f60;
extern int data_0209e650;

extern void func_ov006_020d0ac0(void);
extern void func_ov006_020cd39c(void);
extern void func_ov006_02121bc8(char *self);
extern void func_ov006_021218c4(char *c);
extern int  func_ov006_020ccd04(int *r0);
extern void func_ov006_020cc9fc(char *c);
extern short func_ov006_02121768(char *c);
extern void func_ov006_02121750(char *c, short v);
extern void func_ov006_02120a18(u16 *a, int b);

extern int  _Z14ApproachLinearRiii(int *v, int step, int rate);
extern int  _Z15ApproachLinear2Rsss(short *v, short step, short rate);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int bank);
extern int  RandomIntInternal(int *seed);

void func_ov006_021218fc(char *c)
{
    int old = data_ov006_02140588;
    func_ov006_020d0ac0();
    func_ov006_020cd39c();
    if (data_ov006_02140588 > old) {
        if (data_ov006_02140588 == 3)
            *(u8 *)(c + 0xc3) = 0;
    }
    func_ov006_02121bc8(c);

    if (data_ov006_0213b0ec == 0) {
        *(s16 *)(c + 0x5db8) = 0;
        func_ov006_021218c4(c);
    } else {
        if (_Z14ApproachLinearRiii((int *)(c + 0x5d90), 0, 1)) {
            if (data_ov006_02140588 >= 5 || data_ov006_0214058c < 3) {
                int i;
                char *p = c + 0x500c;
                for (i = 0; i < 4; i++) {
                    if (func_ov006_020ccd04((int *)p)) {
                        func_ov006_020cc9fc(c + 0x500c + i * 0xd0);
                        break;
                    }
                    p += 0xd0;
                }
            }
            *(int *)(c + 0x5d90) = 0x168;
            {
                int amt = data_ov006_02140588 * 2;
                if (amt > 0xb4) amt = 0xb4;
                *(int *)(((int)c + 0x5d90)) -= amt;
            }
        }
    }

    if (data_ov006_02142f60 == 0) {
        if (_Z15ApproachLinear2Rsss((short *)(c + 0x5dbc), 0, 1)) {
            if (func_ov006_02121768(c)) {
                func_ov006_02121750(c, 0);
                func_ov006_02120a18((u16 *)(c + 0x5d84), *(s16 *)(c + 0x5dba));
            } else {
                func_ov006_02121750(c, 1);
                func_ov006_02120a18((u16 *)(c + 0x5d84), *(s16 *)(c + 0x5dba));
            }
            _ZN5Sound12PlayBank2_2DEj(0x1b6);
            {
                unsigned int rnd = (unsigned int)RandomIntInternal(&data_0209e650) & 0x7fffffff;
                int prod = (rnd >> 19) * 0x2d0;
                *(s16 *)(c + 0x5dbc) = (s16)((prod >> 12) + 0x2d0);
                *(s16 *)(c + 0x5dbe) = 0;
                *(s16 *)(c + 0x5dc0) = 1;
            }
        }
    }

    if (*(s16 *)(c + 0x5dbc) <= 0x3c) {
        if (_Z15ApproachLinear2Rsss((short *)(c + 0x5dbe), 0, 1)) {
            *(s16 *)(c + 0x5dbe) = 0xa;
            if (*(s16 *)(c + 0x5dc0) != 0)
                *(s16 *)(c + 0x5dc0) = 0;
            else
                *(s16 *)(c + 0x5dc0) = 1;
        }
    }

    if (*(s16 *)(c + 0x5dba) != 0) {
        _Z14ApproachLinearRiii((int *)(c + 0x5da4), 0, 1);
        _Z14ApproachLinearRiii((int *)(c + 0x5da8), 0x20, 1);
    } else {
        _Z14ApproachLinearRiii((int *)(c + 0x5da4), 0x20, 1);
        _Z14ApproachLinearRiii((int *)(c + 0x5da8), 0, 1);
    }
}
