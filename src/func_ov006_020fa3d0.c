// NONMATCHING: missing logic (ROM does more) (div=33). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int func_ov006_020c1718(int *r0);
extern void func_02012790(int id);
extern void func_ov006_020f94f4(void);
extern int _Z15ApproachLinear2Rsss(short *a, short b, short c);

extern unsigned char data_020a0e40[];
extern unsigned char TOUCH_INPUT_ARR[];
extern unsigned char data_020a0de9[];

int func_ov006_020fa3d0(char *self)
{
    short st = *(short *)(self + 0x5928);
    switch (st) {
    case 4:
        if (func_ov006_020c1718((int *)(self + 0x4f38)) != 0) {
            short *p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    case 5: {
        unsigned char idx = data_020a0e40[0];
        int found = 0;
        if (TOUCH_INPUT_ARR[idx * 4] != 0) {
            if (data_020a0de9[idx * 4] != 0) {
                found = 1;
            }
        }
        if (found != 0) {
            short *p;
            func_02012790(0x62);
            *(short *)(self + 0x592a) = 0x1e;
            func_ov006_020f94f4();
            p = (short *)(self + 0x5928);
            *p += 1;
        }
        break;
    }
    case 6:
    default:
        if (_Z15ApproachLinear2Rsss((short *)(self + 0x592a), 0, 1) != 0) {
            return 1;
        }
        break;
    }
    return 0;
}
