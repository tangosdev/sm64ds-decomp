/* Matrix4x3_ApplyInPlaceToScale at 0x0203c148
 * Builds a scale matrix on the stack (Thumb), then multiplies mF by it in-place.
 */

typedef int Fix12i;

struct Matrix4x3 { char data[0x30]; };

extern void func_020527e8(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z) __attribute__((long_call, target("thumb")));
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);

void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z)
{
    struct Matrix4x3 tmp;
    func_020527e8(&tmp, x, y, z);
    MulMat4x3Mat4x3(&tmp, mF, mF);
}
