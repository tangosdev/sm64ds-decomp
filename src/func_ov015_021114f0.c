// NONMATCHING: 8/95 at exact size 0x17c (was 12). KnockDownPlank drop-shadow scale.
// opt_propagation off is required for the umull/mla/mla prologue.
//
// ONE ROOT, and the other seven words are its knock-on. At +0xb0 the 64-bit product's
// destination pair takes r0, the register the index chain just died in:
//   ROM:  ldrsh r0,[r1,r0] / smull lr,r3,r0,r2   (pair = lr,r3; sv keeps the dead r0)
//   here: ldrsh lr,[r2,r0] / smull r3,r0,lr,r1   (pair = r3,r0; sv pushed out to lr)
// With r0 taken by RdHi, `sv` goes to lr, and the data_02082214 pool address and
// `r4 >> 1` then rotate against each other at +0x84/+0x88 and again at the second
// table read. That is the ROM's compiler declining to reuse a register that died on
// the previous instruction (notes 6bs) -- the direction 6bs records as unreachable
// from source, because the choice is made after every source-level distinction is
// gone. The proof that r0 is the pivot: inline the block-1 table read and `sv` DOES
// take r0, and the pool/half rotation and the ldrsh all come right at once -- but the
// same change drops r7 out of the frame (0x10 vs 0xc, v moves to lr) for six words,
// and both smull pairs then miss, so it scores 14.
//
// MEASURED INERT, first pass: a 128-cell product sweep over {half named|inline} x
// {lookup named|inline} x {v before|after the index chain} x {product named|inline}
// per block, plus const/split/register spellings of `half`, a named table pointer,
// and byte-cast index forms.
// MEASURED INERT, run m100 lane H1 -- 475 further cells, nothing under 8:
//   * naming `half` fixes three words (pool r1, half r2, and the second table read)
//     and costs four at the second smull, netting 9. Naming a table pointer instead is
//     byte-identical to the baseline in both `tbl`-only and `tbl`-then-`half` order,
//     so that name is folded away entirely; only `half` reaches the allocator.
//   * 72 cells of {half named} x {block-1 lookup: inline | new local | written back
//     in place into its own index} x {block-2 the same} x {product named | inline}.
//     Writing the load back into its own index variable scores EXACTLY as a fresh
//     named local, so the source cannot ask for the ROM's `ldrsh r0,[r1,r0]`.
//     Naming the Q12 product is completely inert; hoisting it into an s64 costs 2.
//   * 72 cells of smull operand order and cast placement, 50 of address form
//     (`a[t]`, `*(a+t)`, byte-cast + `t<<1`, `t*2`, `&a[t]`), 21 of the rounding-add
//     and store shapes, 75 of v-position x 64-bit-product form. All canonicalised.
//   * 144 pragma cells: 36 REAL pragma names from mwccarm.exe in both directions, each
//     run twice (baseline and half-named). None below 8; the batch's 28/29/70 outliers
//     show the pragmas were applied.
//   * data_02082214 stays FLAT. Retyping it s16[][2] scores 56 here, which is the
//     fourth independent confirmation in notes 6bv lever 2.
// Cross-build: 1.2/base, 1.2/sp2, 1.2/sp2p3 and 2004/b56 all reach 0x17c and all four
// give the SAME 8; every other build misses the size (1.2/sp3-sp4 0x178, 2.0 0x174 or
// 0x170, dsi 0x150).
#pragma opt_propagation off
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];
int func_ov015_021114f0(char *c) {
    int r5;
    int r4 = 0;
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = *(short*)((char*)data_02082214 + (idx << 2));
    int r2 = (int)(((long long)s * 0x7d0000 + 0x800) >> 12);
    r5 = 0x8000;
    if (r2 <= 0) { r5 = (unsigned short)(r5 + 0x8000); r2 = -r2; }
    r4 = r4 + r2;
    Matrix4x3_FromRotationY(c+0x348, (short)(*(short*)(c+0x8e) + r5));
    {
        int v = *(int*)(c+0x378);
        int t = *(short*)(c+0x8e);
        t = (short)(t + r5);
        t = (unsigned short)t;
        t = t >> 4;
        t = (t << 1) + 1;
        int sv = data_02082214[t];
        *(int*)(c+0x36c) = (v + (int)(((long long)sv * (r4 >> 1) + 0x800) >> 12)) >> 3;
    }
    *(int*)(c+0x370) = *(int*)(c+0x37c) >> 3;
    {
        int v = *(int*)(c+0x380);
        int t = *(short*)(c+0x8e);
        t = (short)(t + r5);
        t = (unsigned short)t;
        t = t >> 4;
        t = t << 1;
        *(int*)(c+0x374) = (v + (int)(((long long)data_02082214[t] * (r4 >> 1) + 0x800) >> 12)) >> 3;
    }
    if (*(unsigned char*)(c+0x397) >= 2)
        *(int*)(c+0x370) = *(int*)(c+0x384) >> 3;
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x320, c+0x348, 0xc8000, 0x12c000, r4, 0xf);
}
