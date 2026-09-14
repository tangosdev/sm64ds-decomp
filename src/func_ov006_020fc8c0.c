// @symbol func_ov006_020fc8c0
/* recovered: Bob-omb Squad (dScMgPachinko_c block): a 30-entry loop over 0x38-byte records, each pushed through func_ov004_020b023c with a table-scaled value. */
// NONMATCHING: div 6 of 60 words. mwccarm 2004/b56, --module ov006,
// @ 0x020fc8c0 size 0xf0. Residue class: 6bs register recycle (shape-exact: every word agrees except register names).
// Draft from nearmiss/db.jsonl (stored divergence 6), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
typedef long long s64;

struct V { int a, b, c, d; };
extern void func_ov004_020b023c(void *a, int b, int c, int d, struct V *v);
extern short data_02082214[];
extern int data_ov006_02136cd4[];

void func_ov006_020fc8c0(char *c)
{
    int i;
    char *o = c;
    for (i = 0; i < 0x1e; i++, o += 0x38) {
        char *r2 = o + 0x4000;
        if (*(unsigned char *)(r2 + 0x68d) != 0) {
            int sb = ((int)*(unsigned short *)(o + 0x4684) >> 4) << 1;
            int ip = *(int *)(r2 + 0x680);
            int bidx = *(unsigned char *)(r2 + 0x693);
            struct V v;
            int v_a = (int)(((s64)data_02082214[sb + 1] * ip + 0x800) >> 0xc);
            int v_b = (int)(((s64)data_02082214[sb] * ip + 0x800) >> 0xc);
            int b = (*(int *)(r2 + 0x660) + *(int *)(r2 + 0x670)) >> 0xc;
            int c2 = (*(int *)(r2 + 0x664) + *(int *)(r2 + 0x674)) >> 0xc;
            v.a = v_a;
            v.b = v_b;
            v.c = -v_b;
            v.d = v_a;
            func_ov004_020b023c(
                (void *) data_ov006_02136cd4[bidx],
                b,
                c2,
                -1,
                &v);
        }
    }
}
