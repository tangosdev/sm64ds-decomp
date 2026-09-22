// func_02007b0c @ 0x02007b0c (size 0x8c) - Camera member.
// Reads a packed offset record (3 unaligned shorts + a byte step) from arg1,
// builds a Fix12i Vector3 offset (each short << 12), rotates it about the
// owner dActor_c's Y angle (+0x8000, facing opposite the player) and translates
// by the owner's position into a result vector, then approaches the camera's
// field_0x80 Vector3 toward that result with a per-axis step (byte6 << 4).

typedef int Fix12i;

typedef struct Vector3 {
    Fix12i x, y, z;
} Vector3;

typedef struct Vector3_16 {
    short x, y, z;
} Vector3_16;

typedef struct dActor_c {
    char _pad0[0x5c];
    Vector3 pos;        // 0x5c
    char _pad1[0x8c - (0x5c + 0xc)];
    Vector3_16 ang;     // 0x8c
} dActor_c;

typedef struct Camera {
    char _pad0[0x80];
    Vector3 field_0x80; // 0x80
    char _pad1[0x110 - (0x80 + 0xc)];
    dActor_c* owner;       // 0x110
} Camera;

typedef struct Packed {
    char x;             // unaligned short @ +0
    char y;             // unaligned short @ +2
    char z;             // unaligned short @ +4
    unsigned char step; // +6
} Packed;

extern short ReadUnalignedShort(const char* from);                                   // 0x0200e768
extern void Vec3_RotateYAndTranslate(Vector3* res, const Vector3* translation,
                                     short angY, const Vector3* v);                   // 0x0203b6a4
extern void func_02007cec(Vector3* dest, const Vector3* src, int step);              // 0x02007cec

int func_02007b0c(Camera* self, const char* rec) {
    Vector3 v;
    Vector3 res;
    dActor_c* owner;
    int step;
    Fix12i vx, vy, vz;
    vz = (Fix12i)ReadUnalignedShort(rec + 4) << 12;
    vy = (Fix12i)ReadUnalignedShort(rec + 2) << 12;
    vx = (Fix12i)ReadUnalignedShort(rec + 0) << 12;
    v.x = vx;
    v.y = vy;
    v.z = vz;
    step = (unsigned char)rec[6] << 4;
    owner = self->owner;
    Vec3_RotateYAndTranslate(&res, &owner->pos, owner->ang.y + 0x8000, &v);
    func_02007cec(&self->field_0x80, &res, step);
    return 1;
}
