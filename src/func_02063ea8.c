typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern void func_0205a61c(int a, void *b, int c);
extern int func_02065a74(void);
extern void func_02064470(void *self, int code, int idx);
extern void func_02064554(void *self, int sel, int idx, int a3);
extern void func_02065ba0(int v);
extern void func_02065bb0(int v);
extern int func_020647d8(void *p, int i);
extern int func_02064674(void *p, int idx, int dst);
extern void func_020647a4(void *p, int value);

int func_02063ea8(char *arg0, int arg1, int arg2, int arg3)
{
    int r7;
    int r6;
    char *r5;
    int off;
    int r8;
    int sp0;
    char *sp4;

    off = arg1 * 0x68;
    sp0 = arg3;
    r5 = arg0 + 0x1d4 + off;
    r6 = 1;
    sp4 = arg0 + 0x1f8 + off;
    r7 = 0;
    r8 = arg2;
    *(u8 *)r5 = 1;
    if (sp0 == 0) {
        return r7;
    }
    if (sp0 > 0) {
        int sp8 = 1;
        int spC = 0;
        int sp10 = 1;
        int sp14 = 0;
        int sp18 = 3;
        int sp1C = 1;
        int sp20 = 5;
        int sp24 = 0xA;
        int sp28 = 7;
        int sp2C = 1;
        int sp30 = 9;
        int sp34 = 8;
        int sp38 = 0xF;
        int r4 = 4;
        int fp = 2;

        for (;;) {
            switch (r6) {
            case 1:
                func_0205a61c(r8, r5 + 1, sp8);
                r8 += 1;
                r7 += 1;
                r6 = fp;
                break;
            case 2:
            {
                u16 t;
                int v;
                func_0205a61c(r8, r5 + 8, fp);
                r8 += 2;
                r7 += 2;
                t = *(u16 *)(r5 + 8);
                if (func_02065a74() == -1) {
                    v = spC;
                } else if (t & (sp10 << func_02065a74())) {
                    v = sp10;
                } else {
                    v = sp14;
                }
                if (v == 0) {
                    return r7;
                }
                r6 = sp18;
                break;
            }
            case 3:
            {
                u8 v;
                func_0205a61c(r8, r5 + 2, sp1C);
                v = *(u8 *)(r5 + 1);
                r8 += 1;
                r7 += 1;
                switch (v) {
                case 1:
                    return r7;
                case 2:
                    r6 = sp20;
                    break;
                case 3:
                    r6 = 4;
                    break;
                case 11:
                    if (*(int *)arg0 == 0xA) {
                        func_02064470(arg0, v, arg1);
                    }
                    return r7;
                case 10:
                    r6 = sp24;
                    break;
                default:
                case 0:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    r6 = sp28;
                    break;
                }
                break;
            }
            case 10:
                func_0205a61c(r8, r5 + 0xD, sp2C);
                r8 += 1;
                r7 += 1;
                r6 = sp30;
                break;
            case 9:
                *(u32 *)(r5 + 0x20) = r8;
                func_02064554(arg0, *(u8 *)(r5 + 1), arg1, 0);
                *(u8 *)r5 = 2;
                return r7 + 9;
            case 4:
                func_0205a61c(r8, r5 + 0xA, fp);
                r8 += 2;
                r7 += 2;
                if (*(u8 *)(r5 + 1) == 3) {
                    r6 = sp20;
                }
                break;
            case 5:
            {
                u8 v;
                func_0205a61c(r8, r5 + 6, 2);
                func_0205a61c(r8 + 2, r5 + 4, 2);
                if (func_02065a74() != 0) {
                    func_02065ba0(*(short *)(r5 + 4));
                    func_02065bb0(*(short *)(r5 + 6));
                }
                v = *(u8 *)(r5 + 1);
                switch (v) {
                case 2:
                    func_02064554(arg0, v, arg1, 0);
                    *(u8 *)r5 = 2;
                    return r7 + 4;
                case 3:
                    if (*(int *)arg0 == 2) {
                        func_02064470(arg0, v, arg1);
                    }
                    return r7 + 4;
                default:
                    func_02064554(arg0, 0xF, arg1, 3);
                    return r7 + 4;
                }
            }
            case 7:
            {
                u8 v;
                func_0205a61c(r8, r5 + 0x10, r4);
                v = *(u8 *)(r5 + 1);
                r8 += 4;
                r7 += 4;
                switch (v) {
                case 8:
                    func_02064554(arg0, v, arg1, 0);
                    *(u8 *)r5 = 2;
                    return r7;
                case 9:
                {
                    int st = *(int *)arg0;
                    if (st == 4) {
                        if (*(u32 *)(arg0 + 0x14) == *(u32 *)(r5 + 0x10)) {
                            func_02064470(arg0, 5, arg1);
                        }
                    } else if (st == 6 && *(u32 *)(arg0 + 0x14) == *(u32 *)(r5 + 0x10)) {
                        func_02064470(arg0, 7, arg1);
                    }
                    return r7;
                }
                case 4:
                case 5:
                case 6:
                case 7:
                    r6 = sp34;
                    break;
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(arg0, sp38, arg1, sp18);
                    break;
                }
                break;
            }
            case 8:
            {
                u8 v;
                func_0205a61c(r8, r5 + 0x14, r4);
                v = *(u8 *)(r5 + 1);
                r8 += 4;
                r7 += 4;
                switch (v) {
                case 4:
                case 6:
                    *(u8 *)r5 = 2;
                    return r7;
                case 5:
                case 7:
                {
                    int st;
                    *(u32 *)(r5 + 0x20) = r8;
                    st = *(int *)arg0;
                    if ((st == 4 || st == 6)
                        && *(u32 *)(r5 + 0x10) == *(u32 *)(arg0 + 0x14)
                        && (*(u16 *)(arg0 + 8) & (u16)(1 << arg1))
                        && func_020647d8(sp4, *(u32 *)(r5 + 0x14)) == 0) {
                        if (func_02064674(sp4, *(u32 *)(r5 + 0x14), *(u32 *)(r5 + 0x20)) == 1) {
                            func_020647a4(sp4, *(u32 *)(r5 + 0x14));
                        } else {
                            func_02064554(arg0, 0xF, arg1, 6);
                        }
                    }
                    return r7;
                }
                default:
                case 0:
                case 1:
                case 2:
                case 3:
                    func_02064554(arg0, sp38, arg1, sp18);
                    break;
                }
                break;
            }
            default:
            case 0:
            case 6:
                func_02064554(arg0, sp38, arg1, r4);
                break;
            }
            if (r7 >= sp0) {
                break;
            }
        }
    }
    func_02064554(arg0, 0xF, arg1, 5);
    return r7;
}
