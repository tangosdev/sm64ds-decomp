// NONMATCHING: ang*-1 emits rsb; ROM has smulbb (16x16 signed mul). div=1
/* func_ov063_0211640c at 0x0211640c (ov063), size 0x2a0
 * Compiler mwccarm 1.2/sp2p3, flags:
 * -O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 * One divergence at 0x5c: `ang = ang * -1` compiles to `rsb r0,r4,#0`;
 * the ROM keeps the 16-bit `smulbb r0,r4,r0`. mwccarm folds *-1 to a
 * negate; only global `#pragma opt_propagation off` yields smulbb but it
 * then perturbs a second negate block (net div=3). Closest clean form kept. */
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

struct Matrix4x3 { int m[12]; };
struct Vec3 { int x, y, z; };

extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void _ZN9ModelBase12ApplyOpacityEj(void *self, u32 a, int z);
extern void func_020167a4(void *p);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *p);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *sm, void *mtx, int a, int b, u32 g);
extern void func_ov063_021160d4(void *c);

extern struct Matrix4x3 MATRIX_SCRATCH_PAPER;

void func_ov063_0211640c(char *c)
{
    struct Vec3 pos, t1, t2;
    s16 ang;

    if (((u32)(*(u16 *)(c + 0x5d4) << 0x1c) >> 0x1f) == 0)
        return;

    pos.x = *(s32 *)(c + 0x5c);
    pos.y = *(s32 *)(c + 0x60);
    pos.z = *(s32 *)(c + 0x64);
    ang = *(s16 *)(c + 0x8e);

    if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
        s16 neg = -1;
        pos.x = pos.x * neg;
        ang = ang * neg;
    }

    if (*(u8 *)(c + 0x5cc) == 3) {
        Vec3_Asr(&t1, &pos, 3);
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, t1.x, t1.y, t1.z);
        *(struct Matrix4x3 *)(c + 0x39c) = MATRIX_SCRATCH_PAPER;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
        func_020167a4(c + 0x380);
        {
            char *m = *(char **)(c + 0x390);
            *(s16 *)(m + 0x1a) = *(s16 *)(c + 0x8c);
            *(s16 *)(m + 0x1c) = (s16)(ang - 0x4000);
            *(s16 *)(m + 0x1e) = *(s16 *)(c + 0x90);
        }
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(c + 0x388);
    } else {
        Vec3_Asr(&t2, &pos, 3);
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, t2.x, t2.y, t2.z);
        Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, ang);
        *(struct Matrix4x3 *)(c + 0x39c) = MATRIX_SCRATCH_PAPER;
        _ZN9ModelBase12ApplyOpacityEj(c + 0x380, (u8)((int)*(u8 *)(c + 0x5c8) >> 3), 1);
    }

    if (*(u8 *)(c + 0x5c8) >= 0x10) {
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
            pos.x >> 3, pos.y >> 3, pos.z >> 3);
        *(struct Matrix4x3 *)(c + 0x4a4) = MATRIX_SCRATCH_PAPER;
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x434, c + 0x4a4, 0x12c000, 0xc8000, 0xf);

        if ((u32)(*(u16 *)(c + 0x5d4) << 0x17) >> 0x1f) {
            int neg = -1;
            pos.x = pos.x * neg;
            Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER,
                pos.x >> 3, pos.y >> 3, pos.z >> 3);
            *(struct Matrix4x3 *)(c + 0x4d4) = MATRIX_SCRATCH_PAPER;
            _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x45c, c + 0x4d4, 0x12c000, 0xc8000, 0xf);
        }
    }

    func_ov063_021160d4(c);
}