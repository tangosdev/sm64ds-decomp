// @symbol func_ov015_021114f0
/* recovered: KnockDownPlank drop-shadow scale and rotation.
 *
 * `opt_foldconstants off` is load-bearing twice over. It keeps the Q12 product in the
 * prologue as the generic 64x64 umull/mla/mla sequence, and it stops the angle update
 * from collapsing: with folding on, `r5 + 0x8000` is pre-added to 0x10000 and the ROM's
 * `addle r0,r5,#0x8000` is never emitted. `r5` must be the 16-bit type so the wrap is
 * the assignment's own truncation rather than a cast the folder can see through.
 */
#pragma opt_foldconstants off
extern void Matrix4x3_FromRotationY(void *m, short ang);
extern int _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void *self, void *sm, void *mtx, int a, int b, int d, unsigned int e);
extern short data_02082214[];
int func_ov015_021114f0(char *c) {
    unsigned short r5;
    int r4 = 0;
    int idx = *(unsigned short*)(c+0x8c) >> 4;
    int s = *(short*)((char*)data_02082214 + (idx << 2));
    int r2 = (int)(((long long)s * 0x7d0000 + 0x800) >> 12);
    r5 = 0x8000;
    if (r2 <= 0) { r5 = r5 + 0x8000; r2 = -r2; }
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
        int sv = data_02082214[t];
        *(int*)(c+0x374) = (v + (int)(((long long)sv * (r4 >> 1) + 0x800) >> 12)) >> 3;
    }
    if (*(unsigned char*)(c+0x397) >= 2)
        *(int*)(c+0x370) = *(int*)(c+0x384) >> 3;
    return _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c+0x320, c+0x348, 0xc8000, 0x12c000, r4, 0xf);
}
