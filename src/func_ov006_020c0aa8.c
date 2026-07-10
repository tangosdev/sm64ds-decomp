/* func_ov006_020c0aa8 at 0x020c0aa8
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

void func_ov006_020c0aa8(struct Camera *self)
{
    self->eye.x = 0x200;
    self->eye.y = 0x9600;
    self->eye.z = -0x22f00;
    self->target.x = -0x100;
    self->target.y = 0x10500;
    self->target.z = 0x1ee00;
    self->angle = 0xbb0;
    func_ov006_020c0134(self);
}
