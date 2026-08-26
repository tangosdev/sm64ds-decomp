// NONMATCHING: register naming only, in two blocks (7 of 235 words differ). No
// instruction is added, removed or reordered anywhere in the body -- the ROM's
// instruction stream and this one are the same 235 instructions in the same order,
// and seven of them name a different register. Size is exact (0x3ac), control flow
// is exact, the 0x54 frame and every local's stack slot are exact, and ALL FIFTEEN
// external call sites land at the ROM's own byte offset with the ROM's own
// destination: func_0203b958 at +0x01c, +0x1f0, +0x200, +0x210, +0x25c, +0x27c and
// +0x29c; func_0203d434 at +0x0ac; Vec2_Len (0x0203d614) at +0x0ec and +0x13c;
// _ZN4cstd4fdivEii (0x02053258) at +0x148; func_0203d630 at +0x154; func_0203d704
// at +0x164; Vec2_Sub (0x0203d6d0) at +0x174; func_ov006_020d01e0 at +0x344. Plus
// the four pool words data_ov006_02140990 / _02140cb4 / _02140cb6 / _02140cae, whose
// extern declarations are ordered here to match the ROM's own pool order -- the byte
// gate wildcards pool words and cannot see that, so it is taken on the linkcheck
// argument, not on a gate figure. Logic verified instruction by instruction against
// the ROM at 0x020d0c38. Run mg12 lane TRM seated it; run mg14 lane GAPS took it
// from 17 divergences to 7.
//
// THE TWO REMAINING DIVERGENCES, both register naming, both measured:
//
//   1. MIDPOINT BLOCK, 5 words at +0x8c, +0x98, +0x9c, +0xa4 and +0xa8. The ROM
//      keeps my in r2 and shifts it in place (asr r2,r2,#1 then lsl r2,r2,#0xc),
//      and puts mid[0] in r1; this build puts my in r3 and mid[0] in r2, so the two
//      stores at sp+0x14 and sp+0x18 name the opposite registers. Same two values,
//      same two stack slots, same order.
//   2. FIRST RECORD STORE, 2 words at +0x348 and +0x350. The ROM loads
//      data_ov006_02140cb4 into r1; this build loads it into r3. The other two
//      record stores name the ROM's own registers (r0, then r1 again).
//
// The previous banner attributed the +0x8c block to the tail-store block. That was
// wrong and is corrected here: the two sets are independent. Closing the tail (below)
// left the +0x8c block exactly where it was.
//
// WHAT CLOSED THE OTHER TEN, each measured against a clean compile, not asserted:
//
//   * Spelling the success return as the array expression rather than `return rec;`
//     is worth SIX words. It is what gives the ROM's r4/r5 colouring at +0x310,
//     +0x32c and +0x33c -- the compiler CSEs the address straight back onto the rec
//     the call already used, so the emitted code still returns the saved pointer.
//   * Routing the three record-field stores through ONE named `short *p` address
//     temporary with a `*(volatile short *)p =` store is worth FOUR more. It is what
//     makes mwccarm interleave pool load / constant / store the way the ROM does
//     instead of batching the three pool loads ahead of the three stores. The
//     volatile is a codegen spelling and changes nothing about the stores: same three
//     destinations, same index register (i * 0x32c), same values 1, 0 and i.
//
// SWEPT AND REJECTED for the remaining seven, every one re-run from a clean compile:
// volatile on the extern declarations (16); the volatile cast without the named
// temporary (11); three distinct pointer temporaries (11); two alternating temporaries
// (11); all eight direct/through-pointer mixes of the three stores (11, except
// all-three-through-one at 7); an explicit index temporary (17); pointer arithmetic
// instead of &arr[] (17); dropping the (short) cast (13); the stores moved before the
// call (26); all store-order permutations (13 and 14); nine spellings of the midpoint
// block including both compound-assignment forms and inlining the temporaries away
// (7 at best, 19 at worst); five declaration positions for the address temporary (7
// flat). The permuter was run for five minutes at -j4 from this source and never beat
// the base score. Statement order and store spelling do not move either block: this
// reads as a rank-pinned colouring floor, not an untried lever.
//
// Counts as decompiled, not matched. No delinks block, by the NONMATCHING seat
// convention (src/MgTrampolineTerror_Spawn.c precedent).
//
// WHAT IT DOES: this is the stylus STROKE-CONNECTED TEST that dScMgTrampoline_c
// and dScMgTrampoline2_c ask on every stylus release (vtable slot 23, via
// src/func_ov006_0212101c.c and src/func_ov006_02122f24.c). Given the stroke's
// two endpoints it rejects strokes narrower than 8 in x, normalises the half
// delta and rejects it if |x| < 0x579 (too steep), stretches a short stroke out
// to a fixed 0x18000 length about its midpoint, then walks the three trampoline
// records looking for a free one: a candidate is rejected if the stroke crosses
// any of the four LIVE records' segments (the two straddle tests are the standard
// signed-area segment intersection). On success it installs the stroke into the
// free record via func_ov006_020d01e0, marks it live, and RETURNS THE RECORD
// POINTER -- which is why slot 23's true arm (the hit sound) fires. Returning 0
// is the miss arm, which is all the port could do while this body was trapped.
extern void func_0203b958(short *o, short *a, short *b);
extern int func_0203d434(int *v);
extern int Vec2_Len(int *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_0203d630(int *p, int m);
extern void func_0203d704(int *o, int *a, int *b);
extern void func_ov006_020d01e0(short *g, short *a, short *b);
extern void Vec2_Sub(int *o, int *a, int *b);

extern unsigned char data_ov006_02140990[];
extern unsigned char data_ov006_02140cb4[];
extern unsigned char data_ov006_02140cb6[];
extern unsigned char data_ov006_02140cae[];

short *func_ov006_020d0c38(short *a, short *b)
{
    short d[2];
    int v1[2];
    int v2[2];
    int mid[2];
    int tmp[2];
    int o1[2];
    int o2[2];
    short s0[2];
    short s1[2];
    short s2[2];
    short t0[2];
    short t1[2];
    short t2[2];
    short pa[2];
    short pb[2];
    int i, j;
    int mx, my;
    short *rec;
    unsigned char *pi;
    unsigned char *pj;
    short *p;

    func_0203b958(d, a, b);
    if ((d[0] < 0 ? (short)-d[0] : d[0]) < 8)
        return 0;

    v1[0] = d[0] << 12;
    v1[1] = d[1] << 12;
    v2[0] = (d[0] >> 1) << 12;
    v2[1] = (d[1] >> 1) << 12;
    mx = (a[0] + b[0]) >> 1;
    my = (a[1] + b[1]) >> 1;
    mid[0] = mx << 12;
    mid[1] = my << 12;

    if (func_0203d434(v2) == 0)
        return 0;
    if ((v2[0] < 0 ? -v2[0] : v2[0]) < 0x579)
        return 0;

    if (Vec2_Len(v1) < 0x30000) {
        tmp[0] = v2[0];
        tmp[1] = v2[1];
        tmp[1] = (int)(((long long)v2[1] * 0xc00 + 0x800) >> 12);
        func_0203d630(v2, _ZN4cstd4fdivEii(0x18000, Vec2_Len(tmp)));
        func_0203d704(o1, mid, v2);
        Vec2_Sub(o2, mid, v2);
        a[0] = (short)(o1[0] >> 12);
        a[1] = (short)(o1[1] >> 12);
        b[0] = (short)(o2[0] >> 12);
        b[1] = (short)(o2[1] >> 12);
    }

    pi = data_ov006_02140990;
    for (i = 0; i < 3; i++, pi += 0x32c) {
        if (pi[0x328] != 0)
            continue;
        pj = data_ov006_02140990;
        for (j = 0; j < 4; j++, pj += 0x32c) {
            if (i == j)
                continue;
            if (pj[0x328] != 1)
                continue;
            func_0203b958(s0, b, a);
            func_0203b958(s1, (short *)(pj + 0x50), a);
            func_0203b958(s2, (short *)(pj + 0x54), a);
            if ((s0[0] * s1[1] - s0[1] * s1[0])
                * (s0[0] * s2[1] - s0[1] * s2[0]) > 0)
                continue;
            func_0203b958(t0, (short *)(pj + 0x54), (short *)(pj + 0x50));
            s0[0] = t0[0];
            s0[1] = t0[1];
            func_0203b958(t1, a, (short *)(pj + 0x50));
            s1[0] = t1[0];
            s1[1] = t1[1];
            func_0203b958(t2, b, (short *)(pj + 0x50));
            s2[0] = t2[0];
            s2[1] = t2[1];
            if ((s0[0] * s1[1] - s0[1] * s1[0])
                * (s0[0] * s2[1] - s0[1] * s2[0]) <= 0)
                return 0;
        }
        pa[0] = a[0];
        pa[1] = a[1];
        pb[0] = b[0];
        pb[1] = b[1];
        rec = (short *)&data_ov006_02140990[i * 0x32c];
        func_ov006_020d01e0(rec, pa, pb);
        p = (short *)&data_ov006_02140cb4[i * 0x32c];
        *(volatile short *)p = 1;
        p = (short *)&data_ov006_02140cb6[i * 0x32c];
        *(volatile short *)p = 0;
        p = (short *)&data_ov006_02140cae[i * 0x32c];
        *(volatile short *)p = (short)i;
        return (short *)&data_ov006_02140990[i * 0x32c];
    }
    return 0;
}
