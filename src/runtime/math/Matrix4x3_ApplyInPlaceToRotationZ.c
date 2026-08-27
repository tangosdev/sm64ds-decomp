/* Matrix4x3_ApplyInPlaceToRotationZ at 0x0203bf90
 * Builds a rotation-Z matrix on the stack, then multiplies mF by it in-place.
 */

typedef int Fix12i;
typedef short s16;

struct Matrix4x3 { char data[0x30]; };

extern void Matrix4x3_FromRotationZ(struct Matrix4x3 *mF, s16 angZ);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);

void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationZ(&tmp, angZ);
    MulMat4x3Mat4x3(&tmp, mF, mF);
}
