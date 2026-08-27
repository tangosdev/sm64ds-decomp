#include "types.h"

extern int func_0203da9c(void);
extern int func_02020168(void);
extern void func_02020304(void);
extern void func_020200e0(void);
extern void func_ov075_0211a148(char *c, int a, int b);
extern void func_02012790(int id);
extern int func_0203d9b4(void);
extern int func_ov075_02116d40(char *c);
extern void func_02020124(void);
extern void func_02020334(void);
extern void func_02020768(char *thiz);

extern int data_ov075_0211d810;
extern u8 data_0209fc5c[];
extern u8 data_020a0e58[];
extern u8 data_020a0de8[];
extern int data_0208ee44;
extern u16 data_020a0e5a[];
extern u8 data_0209b2f0[];

// 6e: keep pointer-induction un-reduced
#pragma opt_strength_reduction off
// 6e: EBB-local CSE
#pragma opt_common_subs off
void func_ov075_0211867c(char *self)
{
    int id;
    int acc;
    int i;
    int zero;
    int one;
    int bit;
    u8 *p;

    id = func_0203da9c();
    if (func_02020168() != 0) {
        func_02020304();
        func_020200e0();
        func_ov075_0211a148(self, (int)&data_ov075_0211d810, 5);
        func_02012790(0x124);
    } else {
        if (func_0203d9b4() != 0) {
            if (*(int *)(self + 0x274) == 0) {
                acc = 0;
                p = data_0209fc5c;
                i = acc;
                zero = acc;
                one = 1;
                do {
                    bit = *p;
                    if (bit != 0) {
                        acc |= *(u16 *)(data_020a0e58 + (i << 2) + 2) & 0xc;
                        bit = data_020a0de8[i << 2];
                        if (bit == 0)
                            goto set0;
                        bit = (data_020a0de8 + (i << 2))[1];
                        if (bit != 0) {
                            bit = one;
                            goto orr;
                        }
                    set0:
                        bit = zero;
                    orr:
                        acc |= bit;
                    }
                    i = i + 1;
                    p = p + 1;
                } while (i < 4);
                {
                    int *timer;
                    timer = (int *)(self + 0x264);
                    *timer = *timer - data_0208ee44;
                    if (*(int *)(self + 0x264) <= 0 ||
                        (*(int *)(self + 0x264) < 0x21c && acc != 0)) {
                        func_02020334();
                        *(int *)(self + 0x274) = 1;
                    }
                }
            } else {
                if (func_ov075_02116d40(self) != 0)
                    func_02020124();
            }
        } else if (*(int *)(self + 0x274) == 0) {
            int *timer;
            timer = (int *)(self + 0x264);
            *timer = *timer - data_0208ee44;
            if (*(int *)(self + 0x264) < 0x1e0) {
                func_02020334();
                *(int *)(self + 0x274) = 1;
            }
        }
        if ((data_020a0e5a[func_0203da9c() * 2] & ~0xc) != 0)
            func_02012790(0xe);
    }

    func_02020768(self + 0x1b4);
    func_02020768(self + 0x1e0);
    func_02020768(self + 0x20c);
    p = data_0209b2f0 + id;
    if (*p == 0)
        func_02020768(self + 0x238);
    if (*(u8 *)(self + 0x1d9) == 0) {
        if (*(u8 *)(self + 0x205) == 0) {
            *(u8 *)(self + 0x1da) = 0;
            *(u8 *)(self + 0x205) = 1;
            *(u8 *)(self + 0x206) = 1;
            if (*p == 0) {
                *(u8 *)(self + 0x231) = 1;
                *(u8 *)(self + 0x232) = 1;
                *(u8 *)(self + 0x25d) = 1;
                *(u8 *)(self + 0x25e) = 1;
            }
        }
    }
}
