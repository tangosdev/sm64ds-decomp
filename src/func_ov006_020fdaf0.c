// @symbol func_ov006_020fdaf0
/* recovered: minigame actor scan -- find the first live entry within 0x18 of
 * the caller's entry and launch it.
 *
 * Walks the 0x30 records of stride 0x38 that begin at self+0x4000. A record is
 * a candidate when its flag byte at +0x4f0c is non-zero and its state byte at
 * +0x4f0d is at least 2. For each candidate the planar distance from record
 * `i`'s position (self + i*0x38 + 0x4660 for x, +0x4664 for z) is taken in
 * whole units (>> 12) and compared against 0x18 through cstd::sqrt of the
 * 64-bit sum of squares. The first record inside that radius wins: the scan
 * writes its heading into self+0x4684+i*0x38, derives a sine/cosine pair from
 * data_02082214 at 0x2000 scale plus a zero velocity pair into the record at
 * self+i*0x38+0x4000+0x668..0x674, plays the sound whose id comes from
 * data_ov006_0212eb94 indexed by the hit record's counter (clamped at 5),
 * spawns the effect through func_ov006_020fb8fc, bumps that counter, and
 * returns. With no hit the loop runs out and the function returns having
 * written nothing.
 *
 * Two spellings are load-bearing for the register colouring in the guard
 * block, and only TOGETHER -- each one alone scores worse than either does
 * apart (16 and 11 against 7):
 *
 *   1. The four record fields in the guard block are reached off `walk` with
 *      the +0x4000 already folded into the constant (0x4f0c, 0x4f0d, 0x4ed8,
 *      0x4edc). Introducing a `char *w = walk + 0x4000` local and using
 *      w+0xf0c... is the same address arithmetic but creates a named web that
 *      takes r2 away from the loaded x.
 *   2. The z difference is computed BEFORE the x difference. mwccarm schedules
 *      the z term first either way, but the source order decides which of the
 *      two competing values reaches the allocator first, and with it whether
 *      the record base lands in r2 and the loaded x in r3 (the cartridge) or
 *      the other way round.
 */
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;

extern int _ZN4cstd4sqrtEy(unsigned long long v);
extern void func_02012718(void *a, int b);
extern void func_ov006_020fb8fc(char *c, int a2, int a3, int a4, int a5, int a6);
extern s16 data_02082214[];
extern int data_ov006_0212eb94[];
extern u16 data_ov006_0213d954[];

void func_ov006_020fdaf0(char *base, int i)
{
    int n = i * 0x38;
    char *o = base;
    int j = 0;
    char *walk = o;
    int *xp = (int *)((o + n) + 0x4660);
    int *zp = (int *)((o + n) + 0x4664);
    do {
        if (*(u8 *)(walk + 0x4f0c) == 0)
            goto next;
        if (*(u8 *)(walk + 0x4f0d) < 2)
            goto next;
        {
            int dy = (*zp - *(int *)(walk + 0x4edc)) >> 12;
            int dx = (*xp - *(int *)(walk + 0x4ed8)) >> 12;
            s64 distsq = (s64)dx * dx + (s64)dy * dy;
            if (_ZN4cstd4sqrtEy((unsigned long long)distsq) > 0x18)
                goto next;
        }
        {
            int m = j * 0x38;
            int five = 5;
            char *ov = (char *)o;
            unsigned char *f0f = (unsigned char *)(o + 0x4f0f);
            char *bn = ov + n;
            char *bm = ov + m;
            char *ip = bn; ip += 0x4000;
            u16 ang;
            s16 s;
            s16 c;
            int vel;
            int cnt;
            int cc;
            char *p84;
            char *p78;
            char *p60;
            unsigned char *cntp;
            int round = 0x800;

            *(u8 *)(ip + 0x68f) = (u8)five;
            {
                char *r = bm + 0x4f00;
                ang = *(u16 *)(r + 8);
                r = o + 0x4684;
                p84 = r;
            }
            *(u16 *)(p84 + n) = ang;
            ang = *(u16 *)(p84 + n);

            s = data_02082214[((ang >> 4) << 1) + 1];
            {
                unsigned char *lr = f0f;
                p78 = o + 0x4678;
                *(int *)(ip + 0x668) = (int)((((s64)s * 0x2000) + round) >> 12);
                ang = *(u16 *)(p84 + n);
                c = data_02082214[(ang >> 4) << 1];
                *(int *)(ip + 0x66c) = (int)((((s64)c * 0x2000) + round) >> 12);
                *(int *)(p78 + n) = 0;
                ang = *(u16 *)(p84 + n);
                vel = *(int *)(p78 + n);
                s = data_02082214[((ang >> 4) << 1) + 1];
                *(int *)(ip + 0x670) = (int)((((s64)s * (s64)vel) + round) >> 12);
                ang = *(u16 *)(p84 + n);
                vel = *(int *)(p78 + n);
                c = data_02082214[(ang >> 4) << 1];
                *(int *)(ip + 0x674) = (int)((((s64)c * (s64)vel) + round) >> 12);
                cnt = *(u8 *)(lr + m);
                cc = cnt;
                if (cc >= 5)
                    cc = five;
                p60 = o + 0x4660;
                func_02012718((void *)data_ov006_0212eb94[cc], *(int *)(p60 + n));
                {
                    char *q = (char *)o + n;
                    func_ov006_020fb8fc(o,
                        *(int *)(p60 + n),
                        *(int *)(q + 0x4664),
                        2,
                        (int)data_ov006_0213d954[cc],
                        0);
                }
                cntp = f0f + m;
                *cntp = (u8)(*cntp + 1);
            }
            return;
        }
    next:
        j += 1;
        walk += 0x38;
    } while (j < 0x30);
}
