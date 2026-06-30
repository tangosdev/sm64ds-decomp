/* func_0200814c at 0x0200814c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef struct Vec3 { int x, y, z; } Vec3;

extern struct Actor *func_0200e6d8(unsigned int arg0);
extern void Vec3_RotateYAndTranslate(Vec3 *out, const Vec3 *trans, short ang, const Vec3 *add);

int func_0200814c(int self)
{
    struct Actor *a;
    int diff, absdiff, val;
    Vec3 v;

    a = func_0200e6d8(0);
    diff = *(int *)((char *)a + 0x60) - *(int *)(self + 0x90);
    v.x = 0;
    v.y = 0x50000;
    v.z = diff;
    absdiff = diff < 0 ? -diff : diff;
    val = (int)(((long long)absdiff * -0x199 + 0x800) >> 12);
    v.z = val;
    if (val > -0x64000)
        v.z = -0x64000;
    if (diff > -0x300000)
        Vec3_RotateYAndTranslate((Vec3 *)(self + 0x80),
                                 (const Vec3 *)((char *)a + 0x5c),
                                 *(short *)((char *)a + 0x8e),
                                 &v);
    return 1;
}