/* CommonModel::Render at 0x02016104
 * Multiplies this->mat4x3 by data_0209b3ec into a stack-local temp matrix,
 * then calls ModelComponents::Render(temp, scale).
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };

/* Matrix3x3 = 3 Vector3 columns = 36 bytes */
struct Matrix3x3 { struct Vector3 c0, c1, c2; };
/* Matrix4x3 = Matrix3x3 + Vector3 = 48 bytes = 0x30 */
struct Matrix4x3 { struct Matrix3x3 base; struct Vector3 c3; };

struct ModelComponents;

struct CommonModel {
    void *vtable;            /* 0x00 */
    unsigned int unk04;      /* 0x04 */
    struct ModelComponents *data;  /* 0x08 */
    struct Matrix4x3 mat4x3; /* 0x0c */
};

extern struct Matrix4x3 data_0209b3ec;  /* 0x0209b3ec */
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF); /* 0x02052914 */
extern void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(struct ModelComponents *thiz, struct Matrix4x3 *mat, const struct Vector3 *scale); /* 0x020443c8 */

void _ZN11CommonModel6RenderEPK7Vector3(struct CommonModel *thiz, const struct Vector3 *scale)
{
    struct Matrix4x3 temp;
    MulMat4x3Mat4x3(&thiz->mat4x3, &data_0209b3ec, &temp);
    _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(thiz->data, &temp, scale);
}
