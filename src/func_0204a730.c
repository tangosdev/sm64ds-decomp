/* func_0204a730 @ 0x0204a730 (arm9, size 0x6fc)   [mwccarm 1.2/sp2p3]
 * Levers that matched (notes 6ap): group B = u32 byte-hoist at function scope with
 * s32 k demoted into the loop block after var_r5; group A = flags declared plain u32
 * but WRITTEN through a volatile cast - the volatile-touched object lands in the
 * stack hole at [sp,#0x2c] while the plain reads stay value-numberable, which blocks
 * the lsl/lsr re-fold. Do not "simplify" the volatile store or the shifts re-fold.
 */
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef long long s64;

typedef struct { s32 x, y, z; } Vec3;

extern void func_0204c584(void *self, void *arg);
extern void func_0204c304(void *node, void *self, void *arg);
extern void *func_0204d8e8(void *listHead, void *node);
extern void func_0204d9a0(void *list, void *node);

typedef void (*Cb1)(void *, void *, u32);
typedef void (*Cb2)(u32, void *, void *);

typedef struct {
    Cb1 func;
    u32 data;
} Ent1;

typedef struct {
    Cb2 func;
    u32 data;
} Ent2;

extern void func_0204d440(void *, void *, u32);
extern void func_0204d294(void *, void *, u32);
extern void func_0204d1b4(void *, void *, u32);
extern void func_0204d150(void *, void *, u32);
extern void func_0204d104(void *, void *, u32);
extern void func_0204d0b8(void *, void *, u32);

#define LP(T, e) ((T *)(int)(((long long)(int)(e))))

#pragma opt_common_subs off

void func_0204a730(void *arg0, void *self)
{

    char *sb;
    void *next;
    char *cfg;
    s32 spC;
    s32 sp10;
    s32 sp14;
    void *next2;
    s32 zero_home;
    u32 flags;
    u16 thresh;
    u16 count2;
    void *var_r4;
    u8 spArr[2];
    Ent1 table1[4];
    Ent1 tableB[4];
    s32 acc[3];
    char *player;
    u32 bb;
    s32 i;
    s32 n;


    sb = *(char **)((char *)self + 0x18);
    cfg = *(char **)(sb + 0x14);
    player = *(char **)(sb + 0);
    *(volatile u32 *)&flags = *(u32 *)player;
    i = 0;
    n = 0;
    spC = (s32)(*(u8 *)(player + 0x32)) + 0x180;
    thresh = *(u16 *)(player + 0x28);
    count2 = *(u16 *)(sb + 0x1c);

    if (thresh == 0 || (u16)(*(u16 *)((char *)self + 0x38)) < thresh) {
        if ((u16)(*(u16 *)((char *)self + 0x38)) % (u8)(*(u8 *)((char *)self + 0x58)) == 0) {
            if (((u32)(*(s32 *)((char *)self + 0x1c) << 0x1f) >> 0x1f) == 0
                && ((u32)(*(s32 *)((char *)self + 0x1c) << 0x1e) >> 0x1f) == 0) {
                func_0204c584(self, (char *)arg0 + 0x14);
            }
        }
    }

    if (((u32)(flags << 0x17) >> 0x1f) != 0) {
        table1[0].func = func_0204d440;
        n += 1;
        {
            char *t = *(char **)(sb + 4);
            table1[0].data = (u32)((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f);
        }
    }
    if (((u32)(flags << 0x16) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 8);
        if (((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f) == 0) {
            s32 idx = n;
            table1[idx].func = func_0204d294;
            {
                u32 d = (u32)((u32)(*(u16 *)(t + 8) << 0x1e) >> 0x1f);
                n = idx + 1;
                table1[idx].data = d;
            }
        }
    }
    if (((u32)(flags << 0x15) >> 0x1f) != 0) {
        s32 idx = n;
        table1[idx].func = func_0204d1b4;
        {
            char *t = *(char **)(sb + 0xc);
            u32 d = (u32)((u32)(*(u16 *)(t + 2) << 0x17) >> 0x1f);
            n = idx + 1;
            table1[idx].data = d;
        }
    }
    if (((u32)(flags << 0x14) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 0x10);
        if (((u32)(*(s32 *)(t + 8) << 0xf) >> 0x1f) == 0) {
            table1[n].func = func_0204d150;
            table1[n].data = (u32)((u32)(*(s32 *)(t + 8) << 0xe) >> 0x1f);
            n += 1;
        }
    }

    var_r4 = *(void **)((char *)self + 8);
    if (var_r4 != 0) {
        u32 f = flags;
        sp10 = (s32)((u32)(f << 0x10) >> 0x1f);
        sp14 = (s32)((u32)(f << 0xf) >> 0x1f);
        zero_home = 0;
        do {
            char *nodeC = (char *)var_r4;
            u16 ratio;
            s32 j;


            next = *(void **)(nodeC + 0);
            ratio = *(u16 *)(nodeC + 0x2e);

            spArr[0] = (u8)((s32)(*(u16 *)(nodeC + 0x3e) * ratio) >> 8);
            spArr[1] = (u8)((s32)(*(u16 *)(nodeC + 0x3c) * ratio) >> 8);

            i = zero_home;
            if (n > 0) {
                do {
                    table1[i].func(var_r4, sb, spArr[table1[i].data]);
                    i += 1;
                } while (i < n);
            }

            j = zero_home;
            acc[2] = zero_home;
            acc[1] = zero_home;
            acc[0] = zero_home;

            if (sp10 != 0) {
                *(Vec3 *)(nodeC + 8) = *(Vec3 *)((char *)self + 0x20);
            }

            if ((s32)count2 > 0) {
                do {
                    Ent2 *tbl2 = *(Ent2 **)(sb + 0x18);
                    tbl2[j].func(tbl2[j].data, var_r4, &acc[0]);
                    j += 1;
                } while (j < (s32)count2);
            }

            {
                u16 *pAng = LP(u16, nodeC + 0x36);
                *pAng = (u16)(*pAng + *(s16 *)(nodeC + 0x38));
            }
            *(s32 *)(nodeC + 0x20) = (*(s32 *)(nodeC + 0x20) * spC) >> 9;
            *(s32 *)(nodeC + 0x24) = (*(s32 *)(nodeC + 0x24) * spC) >> 9;
            *(s32 *)(nodeC + 0x28) = (*(s32 *)(nodeC + 0x28) * spC) >> 9;
            {
                s32 *pvx = LP(s32, nodeC + 0x20);
                s32 *pvy = LP(s32, nodeC + 0x24);
                s32 *pvz = LP(s32, nodeC + 0x28);
                *pvx = *pvx + acc[0];
                *pvy = *pvy + acc[1];
                *pvz = *pvz + acc[2];
            }
            {
                s32 *ppx = LP(s32, nodeC + 0x14);
                s32 *ppy = LP(s32, nodeC + 0x18);
                s32 *ppz = LP(s32, nodeC + 0x1c);
                *ppx = *ppx + (*(s32 *)(nodeC + 0x20) + *(s32 *)((char *)self + 0x2c));
                *ppy = *ppy + (*(s32 *)(nodeC + 0x24) + *(s32 *)((char *)self + 0x30));
                *ppz = *ppz + (*(s32 *)(nodeC + 0x28) + *(s32 *)((char *)self + 0x34));
            }

            if (sp14 != 0) {
                u16 life = *(u16 *)(nodeC + 0x2e);
                s32 a = (s32)(*(u16 *)(nodeC + 0x2c)) << 0xc;
                s32 b = ((s32)(*(u8 *)(cfg + 0xd)) + 1) << 0xc;
                s32 r1 = (s32)(((s64)a * b + 0x800) >> 0xc);
                s32 r0 = ((s32)life << 0xc) - (r1 >> 8);
                if (life != 0 && r0 >= 0 && ((r0 >> 0xc) % (u8)(*(u8 *)(cfg + 0xe))) == 0) {
                    func_0204c304(var_r4, self, (char *)arg0 + 0x14);
                }
            }

            {
                u16 *pT = LP(u16, nodeC + 0x2e);
                *pT = (u16)(*pT + 1);
            }
            if ((u16)(*(u16 *)(nodeC + 0x2e)) > (u16)(*(u16 *)(nodeC + 0x2c))) {
                func_0204d9a0((char *)arg0 + 0x14, func_0204d8e8((char *)self + 8, var_r4));
            }

            var_r4 = next;
        } while (next != 0);
    }

    if (((u32)(flags << 0xf) >> 0x1f) != 0) {
        s32 n2 = 0;
        u16 cflags;
        void *var_r5;
        s32 k;

        cflags = *(u16 *)cfg;
        if (((u32)(cflags << 0x1e) >> 0x1f) != 0) {
            tableB[0].func = func_0204d104;
            tableB[0].data = 0;
            n2 += 1;
        }
        cflags = *(u16 *)cfg;
        if (((u32)(cflags << 0x1d) >> 0x1f) != 0) {
            tableB[n2].func = func_0204d0b8;
            tableB[n2].data = 0;
            n2 += 1;
        }
        cflags = *(u16 *)cfg;
        var_r5 = *(void **)((char *)self + 0x10);
        if (((u32)(cflags << 0x1f) >> 0x1f) == 0) {
            count2 = 0;
        }

        if (var_r5 != 0) {
            zero_home = 0;
            do {
                char *nodeC = (char *)var_r5;
                u16 e2c;
                u16 e2e;

                e2e = *(u16 *)(nodeC + 0x2e);
                next2 = *(void **)(nodeC + 0);
                e2c = *(u16 *)(nodeC + 0x2c);
                spArr[0] = (u8)(((s32)(e2e << 8)) / (s32)e2c);

                k = zero_home;
                if (n2 > 0) {
                    bb = spArr[0];
                    do {
                        tableB[k].func(var_r5, sb, bb);
                        k += 1;
                    } while (k < n2);
                }

                k = zero_home;
                acc[2] = zero_home;
                acc[1] = zero_home;
                acc[0] = zero_home;

                if (((u32)((*(u16 *)cfg) << 0x1a) >> 0x1f) != 0) {
                    *(Vec3 *)(nodeC + 8) = *(Vec3 *)((char *)self + 0x20);
                }

                if ((s32)count2 > 0) {
                    do {
                        Ent2 *tbl2 = *(Ent2 **)(sb + 0x18);
                        tbl2[k].func(tbl2[k].data, var_r5, &acc[0]);
                        k += 1;
                    } while (k < (s32)count2);
                }

                {
                    u16 *pAng = LP(u16, nodeC + 0x36);
                    *pAng = (u16)(*pAng + *(s16 *)(nodeC + 0x38));
                }
                *(s32 *)(nodeC + 0x20) = (*(s32 *)(nodeC + 0x20) * spC) >> 9;
                *(s32 *)(nodeC + 0x24) = (*(s32 *)(nodeC + 0x24) * spC) >> 9;
                *(s32 *)(nodeC + 0x28) = (*(s32 *)(nodeC + 0x28) * spC) >> 9;
                {
                    s32 *pvx = LP(s32, nodeC + 0x20);
                    s32 *pvy = LP(s32, nodeC + 0x24);
                    s32 *pvz = LP(s32, nodeC + 0x28);
                    *pvx = *pvx + acc[0];
                    *pvy = *pvy + acc[1];
                    *pvz = *pvz + acc[2];
                }
                {
                    s32 *ppx = LP(s32, nodeC + 0x14);
                    s32 *ppy = LP(s32, nodeC + 0x18);
                    s32 *ppz = LP(s32, nodeC + 0x1c);
                    *ppx = *ppx + (*(s32 *)(nodeC + 0x20) + *(s32 *)((char *)self + 0x2c));
                    *ppy = *ppy + (*(s32 *)(nodeC + 0x24) + *(s32 *)((char *)self + 0x30));
                    *ppz = *ppz + (*(s32 *)(nodeC + 0x28) + *(s32 *)((char *)self + 0x34));
                }
                {
                    u16 *pT = LP(u16, nodeC + 0x2e);
                    *pT = (u16)(*pT + 1);
                }

                if ((u16)(*(u16 *)(nodeC + 0x2e)) > (u16)(*(u16 *)(nodeC + 0x2c))) {
                    func_0204d9a0((char *)arg0 + 0x14, func_0204d8e8((char *)self + 0x10, var_r5));
                }

                var_r5 = next2;
            } while (next2 != 0);
        }
    }

    {
        u16 *pF = LP(u16, (char *)self + 0x38);
        *pF = (u16)(*pF + 1);
    }

    {
        void (*cb)(void *) = *(void (**)(void *))((char *)self + 0x68);
        if (cb == 0) {
            return;
        }
        cb(self);
    }
}
