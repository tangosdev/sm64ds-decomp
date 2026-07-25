//cpp
/* func_ov006_020f7ee4 at 0x020f7ee4 (ov006), size 0x270
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * Note: event must be passed through to func_ov006_020f8320 (zero-cost, r1 already
 * holds it) - its liveness through case 0 is what pushes the case-0 temps up to
 * r2/r3 as in the ROM. The u64-mask launder keeps the RMW address materialized.
 */

extern "C" {

extern int data_ov006_0213d56c;
extern void *data_ov006_021424fc;
extern void *data_ov006_02142508;
extern short data_ov006_021424ec;
extern int data_ov004_020bf9ec;
extern int data_ov006_021424f4;

extern void func_ov006_020f8320(void *c, int event);
extern int func_ov006_020f8154(void *c);
extern int func_ov006_020f8224(void *a, void *b);
extern unsigned int func_02012790(unsigned int a);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);
extern void _Z14ApproachLinearRiii(int *a, int b, int c);
extern int func_0203d5dc(void *a, void *b);
extern int _Z15ApproachLinear2Rsss(short *a, short b, short c);

void func_ov006_020f7ee4(char *c, int event)
{
    switch (*(unsigned char *)(c + 0x2d)) {
    case 0:
        *(short *)((long long)(c + 0x28) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
        if (*(short *)(c + 0x28) != 0)
            return;
        data_ov006_0213d56c--;
        func_ov006_020f8320(c, event);
        return;

    case 2:
        if (*(short *)(c + 0x2a) != event) {
            func_ov006_020f8320(c, event);
            return;
        }
        if (func_ov006_020f8154(c) == 0)
            return;
        if (data_ov006_021424fc == 0) {
            data_ov006_021424fc = c;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x153);
            return;
        }
        if (func_ov006_020f8224(c, data_ov006_021424fc) != 0) {
            data_ov006_02142508 = c;
            data_ov006_021424ec = 0x20;
            *(unsigned char *)(c + 0x2d) = 3;
            _ZN5Sound12PlayBank2_2DEj(0x154);
            if (data_ov004_020bf9ec == 0)
                data_ov004_020bf9ec = 1;
            return;
        }
        func_02012790(0xe);
        {
            void *c2 = data_ov006_021424fc;
            *((unsigned char *)c2 + 0x2d) = 2;
        }
        data_ov006_021424fc = 0;
        return;

    case 3:
        if (func_ov006_020f8154(c) == 0)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        if (data_ov006_021424fc != (void *)c)
            return;
        if (data_ov006_02142508 != 0)
            return;
        _ZN5Sound12PlayBank2_2DEj(0x155);
        data_ov006_021424fc = 0;
        return;

    case 1:
    case 4:
        _Z14ApproachLinearRiii((int *)(c + 0xc), *(int *)(c + 0x14), *(int *)(c + 0x1c));
        _Z14ApproachLinearRiii((int *)(c + 0x10), *(int *)(c + 0x18), *(int *)(c + 0x20));
        _Z14ApproachLinearRiii((int *)(c + 0x24), 0x4000, 0x300);
        if (func_0203d5dc((void *)(c + 0xc), (void *)(c + 0x14)) != 0)
            return;
        if (*(int *)(c + 0x24) != 0x4000)
            return;
        *(unsigned char *)(c + 0x2d) = 2;
        _Z14ApproachLinearRiii(&data_ov006_021424f4, 0, 1);
        *(int *)(c + 0x1c) = 0;
        *(int *)(c + 0x20) = 0;
        return;

    case 5:
        if (_Z15ApproachLinear2Rsss((short *)(c + 0x28), 0, 1) == 0)
            return;
        _Z14ApproachLinearRiii((int *)(c + 0xc), -0x30000, 0x10000);
        return;

    default:
        break;
    }
}

}
