/* func_ov002_020d3b9c @ 0x020d3b9c size 0x5a0 (ov002) - walk speed/steering update,
 * called from Player::St_Walk_Main. Returns 1 on state change, else 0.
 * NONMATCHING (2026-07-11, from scratch to 358/360): structure fully derived;
 * residual = whole-function c/spd callee-reg swap (ROM: this=r6 spd=r5; cand:
 * this=r5 spd=r6) that decl order, register kw, self-copy, and statement moves
 * do not flip (known wall family: callee register rotation, notes 6e item on
 * rotations), plus its scratch echoes (idx r1/r2, mode r3/r2, s1 r1/r3) and a
 * missing mov r0,r5 (spd=t<<15 coalesced into arg; launder folded), umull-block
 * scheduling, /30-block interleave. Semantics verified against sibling
 * St_Walk_Main/020d413c/020d4d88 signatures. */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

#define AT(p, off) ((void *)(int)(((long long)(int)((char *)(p) + (off))) & 0xFFFFFFFFFFFFFFFFLL))

extern int func_ov002_020bf30c(void *c, int a);
extern int func_ov002_020bf224(void *c, int a, int b);
extern int func_ov002_020bf56c(void *c, int b);
extern void func_ov002_020bf88c(void *c);
extern int func_ov002_020c031c(void *c);
extern int func_ov002_020f035c(unsigned int sel, int r1);
extern int func_0203b0e8(int a, int b);
extern int func_01ffabe4(int a, int b);
extern void ApproachAngle(short *cur, short target, int divisor, int band, int maxStep);
extern void func_ov002_020e30a0(void *c, void *s);
extern void func_ov002_020bedd4(void *c);
extern void func_ov002_020d4540(void *c);
extern void func_ov002_020d454c(void *c);
extern int func_ov002_020d4d88(void *c, int a, int b);
extern void func_ov002_020c18b0(void *c, int a);

extern unsigned char data_020a0e40;
extern short data_0209f4a0[];
extern char data_0209f49c[];
extern char data_0209f4ac[];
extern char data_0209f4ae[];
extern int data_ov002_02110184[];

int func_ov002_020d3b9c(char *c)
{
    char *self = c;
    int acc;
    int spd;
    int idx;
    s16 stick;
    s16 targ;
    int diff;

    spd = 0;
    idx = data_020a0e40 * 0x18;
    stick = *(s16 *)((char *)data_0209f4a0 + idx);
    acc = spd;
    if (stick != 0 && (u16)(*(u16 *)(self + 0x6ce) & 0xc) == 0) {
        *(u16 *)(self + 0x6ac) = 4;
        if (*(u8 *)(data_0209f4ac + idx) == 0) {
            spd = func_ov002_020bf30c(self, 0x14000);
            acc = 0x1000;
            if ((*(u16 *)(data_0209f49c + data_020a0e40 * 0x18) & 0x800) == 0) {
                if (*(u16 *)(self + 0x6a4) != 0)
                    spd = func_ov002_020bf30c(self, 0xa000);
            } else {
                spd = func_ov002_020bf30c(self, 0x28000);
            }
        } else {
            u8 mode = *(u8 *)(data_0209f4ae + idx);
            s16 stick2 = *(s16 *)((char *)data_0209f4a0 + idx);
            if (stick2 <= ((mode != 2) ? 0x471 : 0x555)) {
                spd = 0;
            } else if (stick2 <= ((mode != 2) ? 0x955 : 0xAAA)) {
                int s;
                int t;
                s = (mode != 2) ? 0xa : 0x10;
                t = ((stick2 * ((mode != 2) ? 0x24 : 0x30)) >> 12) - s;
                s = (mode != 2) ? 0xa : 0x10;
                spd = t << 15;
                spd = func_01ffabe4(spd, ((mode != 2) ? 0x15 : 0x20) - s);
            } else {
                int a = func_ov002_020bf30c(self, 0x28000);
                int b = func_ov002_020bf30c(self, 0xa000);
                spd = func_ov002_020bf224(self, a, b);
            }
            acc = 0x1000;
            if (*(s16 *)((char *)data_0209f4a0 + data_020a0e40 * 0x18) >= 0xf00)
                acc = 0x1000;
        }
        if (*(u8 *)(self + 0x6ed) != 0) {
            if ((*(u16 *)(data_0209f49c + data_020a0e40 * 0x18) & 0x800) == 0) {
                *(u8 *)(self + 0x6ed) = 0;
            } else {
                u8 *p;
                spd = (int)(((long long)spd * 0x88888889LL + 0x800) >> 12);
                p = (u8 *)AT(self, 0x6ED);
                *p = (u8)(*p - 1);
                acc = 0x1e000;
                func_ov002_020bf88c(self);
            }
        }
        {
            int v = *(int *)(self + 0x68c);
            if (v > 0xa000) {
                int q = func_01ffabe4(0x6400, v >> 12);
                spd = (int)(((long long)spd * q + 0x800) >> 12);
            }
        }
        *(int *)(self + 0x690) = 0;
        if (*(int *)(self + 8) == 1) {
            u16 h = *(u16 *)(self + 0x6ba);
            if (h != 0 && *(u8 *)(self + 0x6ea) != 0 && h < 0x1e) {
                int a = func_ov002_020bf30c(self, 0x28000);
                int prod = h * a;
                int cap = prod / 30;
                int tmp = -((0x1e - h) << 12) * 2;
                if (spd > cap)
                    spd = cap;
                *(int *)(self + 0x690) = tmp;
            }
        }
        if (func_ov002_020f035c(func_ov002_020c031c(self), *(int *)(self + 0x558)) != 0)
            acc = 0x1000;
        targ = *(s16 *)(self + 0x6d2);
        diff = func_0203b0e8(*(s16 *)(self + 0x94), targ);
        if (diff < 0)
            diff = -diff;
        if (diff > 0x6000) {
            int cur = *(int *)(self + 0x98);
            if (cur >= func_ov002_020bf30c(self, 0x1c000) && *(u8 *)(self + 0x703) == 0) {
                func_ov002_020e30a0(self, data_ov002_02110184);
                func_ov002_020bedd4(self);
                return 1;
            }
            if (cur == 0)
                *(s16 *)(self + 0x94) = targ;
            else
                ApproachAngle((s16 *)(self + 0x94), targ, 4, 0x2000, 0x800);
        } else {
            int dv;
            int step;
            int idx2;
            step = 0x800;
            idx2 = data_020a0e40 * 0x18;
            dv = 4;
            if (*(u8 *)(data_0209f4ac + idx2) == 0) {
                if (*(s16 *)((char *)data_0209f4a0 + idx2) >= 0xa00) {
                    if (func_0203b0e8(*(s16 *)(self + 0x94), targ) >= 0x1000) {
                        if (*(u16 *)(self + 0x6f0) < 0x18) {
                            u16 *p = (u16 *)AT(self, 0x6F0);
                            *p = (u16)(*p + 1);
                            dv = (s16)(0x10 - (*(u16 *)(self + 0x6f0) >> 1));
                        }
                    } else {
                        dv = 0x10;
                        if (*(u16 *)(self + 0x6f0) >= 2) {
                            u16 *p = (u16 *)AT(self, 0x6F0);
                            *p = (u16)(*p - 2);
                        }
                    }
                    step = (s16)((0x10 - (dv - 4)) << 3);
                } else {
                    if (*(u16 *)(self + 0x6f0) >= 2) {
                        u16 *p = (u16 *)AT(self, 0x6F0);
                        *p = (u16)(*p - 2);
                    }
                }
            }
            ApproachAngle((s16 *)(self + 0x94), targ, dv, 0x2000, step);
        }
        if (*(u8 *)(self + 0x6e0) != 0)
            func_ov002_020d4540(self);
    } else {
        *(u8 *)(self + 0x6ed) = 0;
        if (*(u8 *)(self + 0x6e0) != 0) {
            acc = func_ov002_020bf56c(self, 0x2000);
            func_ov002_020bf88c(self);
        } else if (*(u16 *)(self + 0x6ac) == 0) {
            acc = (*(u8 *)(self + 0x703) == 0) ? 0x1000 : 0x4000;
            if (*(int *)(self + 0x98) < -0xa000)
                *(int *)(self + 0x98) = -0xa000;
            if (*(u8 *)(self + 0x703) == 0) {
                if (*(int *)(self + 0x98) > func_ov002_020bf30c(self, 0x10000))
                    func_ov002_020d454c(self);
            }
        }
        if (*(int *)(self + 0x98) < func_ov002_020bf30c(self, 0xa000))
            *(u16 *)(self + 0x6a4) = 0x3c;
    }
    func_ov002_020d4d88(self, spd, acc);
    func_ov002_020c18b0(self, 1);
    *(u16 *)(self + 0x90) = 0;
    return 0;
}

