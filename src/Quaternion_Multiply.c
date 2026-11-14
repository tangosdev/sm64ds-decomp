typedef signed int s32;
typedef signed long long s64;

struct Quat {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
};

#define FX_MUL(a, b) ((s32)(((a) * (b) + 0x800) >> 12))

/* out = q1 * q2, the Hamilton product of two fx32 quaternions stored
   (x, y, z, w) with w the scalar part, each term rounded as an fx32 multiply.
   out may alias either input: every component is read before any is written.
   ov006's FallingPetal (unit020bfec0.cpp) composes its rotation with it. */
void Quaternion_Multiply(struct Quat *q1, struct Quat *q2, struct Quat *out) {
    /* Each component is fetched from q2 and then q1, in x, w, y, z order. This order decides which register each operand gets. */
    s64 x2 = q2->x, x1 = q1->x;
    s64 w2 = q2->w, w1 = q1->w;
    s64 y2 = q2->y, y1 = q1->y;
    s64 z2 = q2->z, z1 = q1->z;

    out->x = FX_MUL(w1, x2) + FX_MUL(x1, w2) + FX_MUL(y1, z2) - FX_MUL(z1, y2);
    out->y = FX_MUL(w1, y2) + FX_MUL(y1, w2) + FX_MUL(z1, x2) - FX_MUL(x1, z2);
    out->z = FX_MUL(w1, z2) + FX_MUL(z1, w2) + FX_MUL(x1, y2) - FX_MUL(y1, x2);
    out->w = FX_MUL(w1, w2) - FX_MUL(x1, x2) - FX_MUL(y1, y2) - FX_MUL(z1, z2);
}
