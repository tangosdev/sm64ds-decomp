// NONMATCHING: base materialization / addressing (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };

extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
extern struct Mtx43 MATRIX_SCRATCH_PAPER;

struct Sub { char pad[0x158]; };

void func_ov075_021151b4(char *c, int idx)
{
    struct Vec3 *src = (struct Vec3 *)(c + 0x920 + idx * 0x158 + 0x118);
    struct Vec3 v;
    struct Vec3 out;
    v.x = src->x;
    v.y = src->y + 0x32000;
    v.z = src->z;
    Vec3_Asr(&out, &v, 3);
    Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, out.x, out.y, out.z);
    *(struct Mtx43 *)(c + 0x8d8) = MATRIX_SCRATCH_PAPER;
}
