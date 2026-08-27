/* func_02007c14 @ 0x02007c14, size 0x88, ARM.
 * Builds an offset vector from a magnitude and an angle: takes a Fix12i
 * magnitude `mag` and an angle `ang`, looks up sin/cos in data_02082214, and
 * forms the local vector {0, mag*Sin, mag*Cos} (Fix12 multiply, round
 * 0x800>>12), then rotates that by `angY` about Y and translates by `trans`,
 * writing the result to `res` via Vec3_RotateYAndTranslate.
 */

typedef int Fix12i; /* 20.12 fixed point */

typedef struct Vector3 {
    Fix12i x, y, z;
} Vector3;

extern const short data_02082214[]; /* interleaved sin/cos pairs @ 0x02082214 */

extern void Vec3_RotateYAndTranslate(Vector3 *res, const Vector3 *trans,
                                     short angY, const Vector3 *v);

void func_02007c14(Vector3 *res, const Vector3 *trans, Fix12i mag,
                   short ang, short angY) {
    Vector3 v;
    unsigned idx = (unsigned short)ang >> 4;

    v.x = 0;
    v.y = (int)(((long long)mag * data_02082214[idx * 2] + 0x800) >> 12);
    v.z = (int)(((long long)mag * data_02082214[idx * 2 + 1] + 0x800) >> 12);
    Vec3_RotateYAndTranslate(res, trans, angY, &v);
}
