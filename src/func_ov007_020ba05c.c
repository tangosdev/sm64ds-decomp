// NONMATCHING: 16/161 at exact size 0x284. The previous draft here was 0x22c -- 88 bytes
// SHORT and semantically wrong (array24/array28 were read off the wrong shadow struct),
// so it was an incomplete reconstruction rather than a near-miss. This is a fresh
// size-exact reconstruction and the whole residue is two callee-saved pair swaps in the
// path-node block at +0x104..+0x15c: the ROM colours the node pointer r7 and the node
// count r8 (and the two distance accumulators sb/sl) where every build here takes the
// mirror assignment. MEASURED INERT: all 120 declaration permutations of the block's five
// locals with split declarations, and all 360 initialised-declaration forms (five
// initialised declarations x the six positions of the `r5 = 0` statement) -- consistent
// with notes 6bs, where rank stops biting once the declaration is split from the
// assignment. Both sides take fresh registers here, so this is a colouring-priority
// difference and not the recycle rule.
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;

extern char *data_ov007_02104ba0;
extern char *data_ov007_0210342c;

extern int func_ov007_020c1da0(int i);
extern void func_ov007_020c6724(char *self, int flag);
extern int func_ov007_020c65bc(char *t, int a1, int a2, int a3);
extern void func_ov007_020bdbcc(int a0, int a1);
extern int func_ov007_020c6550(char *c);
extern int func_ov007_020bb09c(void);

int func_ov007_020ba05c(void)
{
    char *mgr = data_ov007_02104ba0;
    int r6 = 0;
    int r5;
    char *r4 = *(char **)(data_ov007_0210342c + 0x50);

    if (*(int *)(mgr + 0x30) != 0 && *(u16 *)(r4 + 0xc) != 0 && *(u16 *)(r4 + 0x14) == 0 &&
        *(u32 *)(r4 + 0x24) >= 1 && func_ov007_020c1da0(0) == 0) {
        *(int *)(data_ov007_02104ba0 + 0x30) = 0;
        func_ov007_020c6724(*(char **)data_ov007_02104ba0, 0);
        *(int *)(data_ov007_02104ba0 + 0x2c) = 0;
    }
    {
        char *m = data_ov007_02104ba0;
        char *o = *(char **)(*(char **)m + 0x20);
        if (*(u16 *)(o + 8) == 0)
            *(int *)(m + 0x34) = 0;
        else
            *(int *)(m + 0x34) += 0x1000;
    }
    if (func_ov007_020c1da0(0) == 0) {
        char *r3 = data_ov007_02104ba0;
        int st = *(s16 *)(*(char **)(r3 + 8));
        if (st != 6 && st != 7 && *(int *)(r3 + 0x30) == 0) {
            if (*(u16 *)(r4 + 0xc) != 0) {
                unsigned int cnt;
                char *o;
                int flag;
                int y;
                int x;
                o = *(char **)(*(char **)r3 + 0x20);
                r5 = 0;
                flag = 0;
                cnt = *(u16 *)(o + 8);
                y = *(u16 *)(r4 + 8);
                x = *(u16 *)(r4 + 0xa);
                if (cnt >= 2) {
                    int idx = cnt - 2;
                    int dz = (((int *)*(char **)(o + 0x28))[idx] >> 12) - x;
                    int rz = *(s16 *)(r4 + 0x3e);
                    int dx = (((int *)*(char **)(o + 0x24))[idx] >> 12) - y;
                    int rx = *(s16 *)(r4 + 0x3c);
                    int dd = rx * rx + rz * rz;
                    int dd2 = dx * dx + dz * dz;
                    if (dd >= 0x100 && dd2 <= 4)
                        flag = 1;
                }
                if (flag == 0) {
                    r5 = func_ov007_020c65bc(*(char **)r3, y, x, *(int *)(r3 + 0x34) >> 12);
                    func_ov007_020bdbcc(*(s16 *)(r4 + 0x3c), *(s16 *)(r4 + 0x3e));
                } else {
                    char *obj = *(char **)(data_ov007_0210342c + 0x50);
                    ((int *)obj)[11] = *(int *)(obj + 0x2c) | 1;
                }
                if (r5 == 2) {
                    func_ov007_020c6550(*(char **)data_ov007_02104ba0);
                    *(int *)(data_ov007_02104ba0 + 0x34) = 0;
                    r6 = 1;
                } else if (r5 == 1) {
                    char *obj = *(char **)(data_ov007_0210342c + 0x50);
                    *(int *)(obj + 0x28) = 0;
                }
            } else if (*(u16 *)(r4 + 0xc) == 0) {
                char *obj = *(char **)(data_ov007_0210342c + 0x50);
                if (*(u32 *)(obj + 0x24) == 1) {
                    int res = func_ov007_020c6550(*(char **)r3);
                    if (res != 0) {
                        if (res == 2)
                            r6 = 1;
                        r4 = data_ov007_02104ba0;
                        *(int *)(r4 + 0x2c) = *(int *)(*(char **)r4 + 8);
                        *(int *)(data_ov007_02104ba0 + 0x34) = 0;
                        obj = *(char **)(data_ov007_0210342c + 0x50);
                        ((int *)obj)[11] = *(int *)(obj + 0x2c) | 1;
                    }
                }
            }
        }
    }
    func_ov007_020bb09c();
    return r6;
}
