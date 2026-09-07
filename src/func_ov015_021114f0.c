// NONMATCHING: 8/95 at exact size (was 12). KnockDownPlank drop-shadow scale.
// Residue is two register-colouring sites and nothing else:
//   * +0x84/+0x88 the ROM puts the data_02082214 pool address in r1 and r4>>1 in r2;
//     with `half` named those two land correctly but the second Q12 multiply then takes
//     the named operand as the smull Rm, which costs four more words -- 9 instead of 8.
//   * +0xb0 the ROM writes the first table load straight back into its own index
//     register (`ldrsh r0,[r1,r0]`) while every build here takes a fresh lr, which
//     rotates the whole smull/adds/adc/lsr chain that follows (notes 6bs).
// Measured: a 128-cell product sweep over {half named|inline} x {lookup named|inline} x
// {v before|after the index chain} x {product named|inline} per block, plus const/split/
// register spellings of `half`, a named table pointer, and byte-cast index forms.
// opt_propagation off is required for the umull/mla/mla prologue.
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
