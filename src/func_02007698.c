/* func_02007698 at 0x02007698
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int Fix12i;

typedef struct Vector3 {
    Fix12i x, y, z;
} Vector3;

typedef struct Vector3_16 {
    short x, y, z;
} Vector3_16;

typedef struct Actor {
    char _pad0[0x5c];
    Vector3 pos;        // 0x5c
    char _pad1[0x8c - (0x5c + 0xc)];
    Vector3_16 ang;     // 0x8c
} Actor;

typedef struct Camera {
    char _pad0[0x80];
    Vector3 field_0x80; // 0x80
    Vector3 field_0x8c; // 0x8c
    char _pad1[0x110 - (0x8c + 0xc)];
    Actor* owner;       // 0x110
} Camera;

extern short ReadUnalignedShort(const char* from);
extern void Vec3_RotateYAndTranslate(Vector3* res, const Vector3* translation,
                                     short angY, const Vector3* v);

int func_02007698(Camera* self, const char* rec) {
    Vector3 v;
    Vector3 w;
    Actor* owner;
    short angle;
    Vector3* pos;
    int vx, vy, vz;
    int wx, wy, wz;

    vz = (Fix12i)ReadUnalignedShort(rec + 4) << 12;
    vy = (Fix12i)ReadUnalignedShort(rec + 2) << 12;
    vx = (Fix12i)ReadUnalignedShort(rec + 0) << 12;
    v.x = vx;
    v.y = vy;
    v.z = vz;

    wz = (Fix12i)ReadUnalignedShort(rec + 0xa) << 12;
    wy = (Fix12i)ReadUnalignedShort(rec + 8) << 12;
    wx = (Fix12i)ReadUnalignedShort(rec + 6) << 12;
    w.x = wx;
    w.y = wy;
    w.z = wz;

    owner = self->owner;
    pos = &owner->pos;
    angle = owner->ang.y + 0x8000;
    Vec3_RotateYAndTranslate(&self->field_0x80, pos, angle, &v);
    Vec3_RotateYAndTranslate(&self->field_0x8c, pos, angle, &w);
    return 1;
}