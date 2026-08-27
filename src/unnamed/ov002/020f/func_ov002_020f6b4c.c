/* func_ov002_020f6b4c at 0x020f6b4c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

typedef short s16;

struct Vector3 { int z, y, x; };

extern short ReadUnalignedShort(unsigned char *p);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);

struct Obj {
    char pad5c[0x5c];
    struct Vector3 v5c;   /* +0x5c */
    char pad8e[0x8e - (0x5c + 0xc)];
    s16 f8e;              /* +0x8e */
    char pad94[0x94 - 0x90];
    s16 f94;              /* +0x94 */
    char pad98[0x98 - 0x96];
    int f98;             /* +0x98 */
};

int func_ov002_020f6b4c(struct Obj *self, unsigned char *p) {
    struct Vector3 v;
    int y, x;
    x = ReadUnalignedShort(p + 4) << 12;
    y = ReadUnalignedShort(p + 2) << 12;
    v.z = ReadUnalignedShort(p) << 12;
    v.y = y;
    v.x = x;
    self->f8e = Vec3_HorzAngle(&self->v5c, &v);
    self->f94 = self->f8e;
    self->f98 = ReadUnalignedShort(p + 6);
    return 1;
}
