/* Matrix4x3_ApplyInPlaceToRotationX at 0x0203bff0
 * Builds a rotation-X matrix on the stack, then multiplies mF by it in-place.
 */

typedef int Fix12i;
typedef short s16;

struct Matrix4x3 { char data[0x30]; };

extern void Matrix4x3_FromRotationX(struct Matrix4x3 *mF, s16 angX);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);

void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationX(&tmp, angX);
    MulMat4x3Mat4x3(&tmp, mF, mF);
}
