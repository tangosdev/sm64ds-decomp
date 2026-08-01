/* Model::Render at 0x02016b78
 * Multiplies this->mat4x3 by data_0209b3ec into a stack-local temp matrix,
 * then calls ModelComponents::Render(&this->data, temp, scale).
 */

typedef int Fix12i;

struct Vector3 { Fix12i x, y, z; };

struct Matrix3x3 { struct Vector3 c0, c1, c2; };
struct Matrix4x3 { struct Matrix3x3 base; struct Vector3 c3; };

struct ModelComponents {
    void *modelFile;  /* 0x00 */
    void *materials;  /* 0x04 */
    void *bones;      /* 0x08 */
    void *transforms; /* 0x0c */
    void *unk10;      /* 0x10 */
};

struct Model {
    void *vtable;              /* 0x00 */
    unsigned int unk04;        /* 0x04 */
    struct ModelComponents data; /* 0x08, size 0x14 */
    struct Matrix4x3 mat4x3;   /* 0x1c, size 0x30 */
    struct Matrix4x3 *unkMatPtr; /* 0x4c */
};

extern struct Matrix4x3 data_0209b3ec;  /* 0x0209b3ec */
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF); /* 0x02052914 */
extern void _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(struct ModelComponents *thiz, struct Matrix4x3 *mat, const struct Vector3 *scale); /* 0x020443c8 */

void _ZN5Model6RenderEPK7Vector3(struct Model *thiz, const struct Vector3 *scale)
{
    struct Matrix4x3 temp;
    MulMat4x3Mat4x3(&thiz->mat4x3, &data_0209b3ec, &temp);
    _ZN15ModelComponents6RenderEP9Matrix4x3P7Vector3(&thiz->data, &temp, scale);
}
