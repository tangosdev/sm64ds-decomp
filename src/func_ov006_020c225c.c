/* func_ov006_020c225c at 0x020c225c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov006).
 */
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

extern void func_ov006_020c0134(struct Camera *self);

void func_ov006_020c225c(struct Camera *self)
{
    self->eye.x = 0;
    self->eye.y = 0;
    self->eye.z = 0;
    self->target.x = 0;
    self->target.y = 0;
    self->target.z = 0x2d000;
    self->angle = 0x800;
    func_ov006_020c0134(self);
}
