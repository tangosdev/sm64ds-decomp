/* func_ov006_020c3b2c at 0x020c3b2c
 *
 * Camera preset init: sets eye/target vectors and angle, then
 * tail-calls func_ov006_020c0134. Sibling of func_ov006_020c225c.
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

void func_ov006_020c3b2c(struct Camera *self)
{
    self->eye.x = 0x1b000;
    self->eye.y = 0x17600;
    self->eye.z = -0x22f00;
    self->target.x = 0x38000;
    self->target.y = 0x22500;
    self->target.z = -0x51200;
    self->angle = 0xb30;
    func_ov006_020c0134(self);
}
