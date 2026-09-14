// @symbol func_ov006_0211e72c
/* recovered: Hide and Boo Seek (dScMgTeresa_c block): draws a run of sprites through OAM::Render from a table of attribute pointers. */
// NONMATCHING: div 27 of 43 words. mwccarm 2004/b56, --module ov006,
// @ 0x0211e72c size 0xac. Residue class: colouring (CRK-C correctness replacement of an earlier draft).
// Draft from nearmiss/db.jsonl (stored divergence 27), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int draw, void *attr, int x, int y, int palette, int priority,
    int sx, int sy, int a9, int a10);
extern void *data_ov006_0213a964[];

void func_ov006_0211e72c(char *base)
{
    int r5;
    char *r6 = base;
    int r7;
    int sb;
    int r4;
    int r8;
    void **fp = data_ov006_0213a964;
    for (r5 = 0; r5 < 0x10; r5++) {
        char *o = r6 + 0x4000;
        if (*(unsigned char *)(o + 0x67a) != 0) {
            int sl, v2, lr, ip, v1, r0;
            r7 = 0;
            sb = 1;
            r4 = -1;
            r8 = 0x1000;
            sl = *(unsigned char *)(o + 0x676);
            v2 = *(int *)(o + 0x660);
            lr = *(unsigned char *)(o + 0x67d);
            ip = *(unsigned char *)(o + 0x67e);
            v1 = *(int *)(o + 0x664);
            r0 = *(unsigned char *)(o + 0x67b);
            lr = ip * 7 + lr;
            ip = (sl != 0) ? sb : r4;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                sb, fp[lr], v2 >> 0xc, v1 >> 0xc, r4, r0, r8, r8, r7, ip);
        }
        r6 += 0x24;
    }
}
