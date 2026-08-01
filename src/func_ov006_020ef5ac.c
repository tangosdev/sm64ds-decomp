#pragma opt_propagation off
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern int _Z15ApproachLinear2Rsss(s16 *a, s16 b, s16 c);
extern void func_ov006_020c4148(void);
extern void func_ov006_020c6f8c(int a);
extern void func_ov006_020c42bc(void);
extern void func_ov006_020c712c(void);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020ef580(char *self);

extern int data_ov006_02140308;
extern int data_ov006_02140304;
extern int data_ov006_02140328;
extern int data_ov006_02140428;

void func_ov006_020ef5ac(char *self0)
{
    s16 flag;
    char *self = self0;
    int saved = data_ov006_02140308;
    int *vp = (int *)(((int)self + 0x5a64) & 0xFFFFFFFFFFFFFFFFLL);
    int c;

    *vp += *(int *)(self + 0x5a68) >> 12;
    if (*(int *)(self + 0x5a64) > 0x1000)
        *vp -= 0x1000;

    flag = 0;
    c = data_ov006_02140304;
    switch (c) {
    case 0:
        flag = 1;
        break;
    case 1:
        {
            int rc = _Z15ApproachLinear2Rsss((s16 *)(self + 0x5a74), flag, 2);
            if (rc)
                flag = 1;
        }
        break;
    default:
        {
            int rc = _Z15ApproachLinear2Rsss((s16 *)(self + 0x5a74), flag, 1);
            if (rc)
                flag = 1;
        }
        break;
    }

    if (flag != 0 && data_ov006_02140304 < data_ov006_02140328) {
        int t;
        s16 v;
        func_ov006_020c4148();
        *(s16 *)(self + 0x5a74) = 0xf0 - data_ov006_02140308 * 6;
        v = *(s16 *)(self + 0x5a74);
        if (v < 0x3c)
            v = 0x3c;
        *(s16 *)(self + 0x5a74) = v;
        t = (data_ov006_02140308 << 4) + 0x1600;
        if (t > 0x2200)
            t = 0x2200;
        func_ov006_020c6f8c(t);
    }

    func_ov006_020c42bc();
    func_ov006_020c712c();

    if (saved != data_ov006_02140308) {
        switch (data_ov006_02140308) {
        case 3:
            data_ov006_02140328 = 4;
            *(char *)(self + 0xc3) = 0;
            break;
        case 0xa:
            data_ov006_02140328 = 5;
            break;
        case 0x14:
            data_ov006_02140328 = 6;
            break;
        }
        func_ov004_020adb1c(data_ov006_02140308);
    }

    if (data_ov006_02140428 != 0)
        return;
    func_ov006_020ef580(self);
}