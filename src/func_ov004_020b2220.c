// @symbol func_ov004_020b2220
/* recovered: minigame framework (dScMgBase_c block): draws a score number rotated by an angle through a 2x2 matrix and the sprite chain. */
// NONMATCHING: div 33 of 137 words. mwccarm 2004/b56, --module ov004,
// @ 0x020b2220 size 0x224. Residue class: colouring; TU member ov004/unit020b0a38.
// Draft from nearmiss/db.jsonl (stored divergence 33), landed 2026-09-14 under Tango's ruling that the functionally-equivalent C
// drafts live on main with an honest banner so the port and readers have source. Logic is
// verified only as far as the residue class implies: register names and instruction order
// for the shape-exact rows, NOT an independent execution audit for the others. Counts as
// decompiled, not matched; tools/enroll.py leaves it out of the ROM build, which keeps the
// original bytes for this range. A byte-exact match replaces this file and drops the banner.
typedef unsigned short u16;
typedef long long s64;
typedef short s16;
struct M { int _00, _01, _10, _11; };
extern void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5);
extern s16 data_02082214[];
extern int data_ov006_02137cd8[];
#pragma opt_propagation off
void func_ov004_020b2220(int sl, int sb, int value, int r7, int r6, int fix, u16 angle)
{
    int th = 0, te = 0, hu = 0;
    struct M m;
    int idx;
    int cos1, sin1;
    if (value >= 0x270f) value = 0x270f;
    if (value >= 0x3e8) { do { value -= 0x3e8; th++; } while (value >= 0x3e8); }
    if (value >= 0x64) { do { value -= 0x64; hu++; } while (value >= 0x64); }
    if (value >= 0xa) { do { value -= 0xa; te++; } while (value >= 0xa); }
    idx = angle >> 4;
    cos1 = data_02082214[idx * 2 + 1];
    sin1 = data_02082214[idx * 2];
    m._00 = (int)(((s64)cos1 * fix + 0x800) >> 12);
    m._01 = (int)(((s64)sin1 * fix + 0x800) >> 12);
    m._10 = -m._01;
    m._11 = m._00;
    if (th != 0) {
        func_ov004_020b1c68((void *) data_ov006_02137cd8[th], sl - 0x30, sb, r7, r6, &m);
        func_ov004_020b1c68((void *) data_ov006_02137cd8[hu], sl - 0x10, sb, r7, r6, &m);
        func_ov004_020b1c68((void *) data_ov006_02137cd8[te], sl + 0x10, sb, r7, r6, &m);
        sl += 0x30;
    } else if (hu != 0) {
        func_ov004_020b1c68((void *) data_ov006_02137cd8[hu], sl - 0x20, sb, r7, r6, &m);
        func_ov004_020b1c68((void *) data_ov006_02137cd8[te], sl, sb, r7, r6, &m);
        sl += 0x20;
    } else if (te != 0) {
        func_ov004_020b1c68((void *) data_ov006_02137cd8[te], sl - 0x10, sb, r7, r6, &m);
        sl += 0x10;
    }
    func_ov004_020b1c68((void *) data_ov006_02137cd8[value], sl, sb, r7, r6, &m);
}
