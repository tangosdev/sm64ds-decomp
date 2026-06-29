/* func_ov006_020c0134 at 0x020c0134
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
typedef int s32;

struct Vector3 { int x, y, z; };
struct Matrix4x3 { int data[12]; };

struct Camera {
    struct Matrix4x3 viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3 projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};

extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern int NormalizeVec3IfNonZero(struct Vector3 *v);
extern void func_0203cebc(struct Vector3 *out, struct Vector3 *tmp, struct Vector3 *a, struct Vector3 *b);
extern int _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(int,int,int,int,int,int,int,struct Matrix4x3 *);
extern int _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(struct Vector3 *,struct Vector3 *,struct Vector3 *,int,struct Matrix4x3 *);
extern void _Z13CopyToViewMatPK9Matrix4x3(struct Matrix4x3 *);
extern int _ZN7Clipper13Func_020156DCEv(void *,int,int,int,int);

extern short data_02082214[];
extern char data_0209f43c[];

void func_ov006_020c0134(struct Camera *self)
{
    struct Vector3 up;
    struct Vector3 v1c;
    struct Vector3 v28;
    struct Vector3 v34;
    struct Vector3 v40;
    int idx;

    up.x = 0;
    up.y = 0x1000;
    up.z = 0;

    SubVec3(&self->eye, &self->target, &v1c);
    if (NormalizeVec3IfNonZero(&v1c)) {
        func_0203cebc(&v34, &v28, &v1c, &up);
        if (NormalizeVec3IfNonZero(&v28)) {
            func_0203cebc(&v40, &up, &v28, &v1c);
        } else {
            up.x = 0;
            up.y = 0;
            up.z = 0x1000;
        }
    }

    idx = (self->angle >> 4) * 2;
    _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
        data_02082214[idx], data_02082214[idx + 1],
        0x1555, 0x1000, 0x1388000, 0x1000, 1, &self->projMat);

    _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(
        &self->target, &up, &self->eye, 1, &self->viewMat);

    _Z13CopyToViewMatPK9Matrix4x3(&self->viewMat);

    _ZN7Clipper13Func_020156DCEv(
        data_0209f43c, 0x1555, self->angle, 0x1000, 0x1388000);
}
