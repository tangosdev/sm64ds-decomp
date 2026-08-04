// @symbol func_ov006_020c1eb4
/* recovered: shared common types */
#include "common.h"
/* func_ov006_020c1eb4 at 0x020c1eb4
 *
 * Camera preset init: sets eye/target vectors and angle, then
 * tail-calls Camera_UpdateMatrices. Sibling of func_ov006_020c225c.
 */

struct Matrix4x3_local { int data[12]; };

struct Camera {
    struct Matrix4x3_local viewMat;  /* 0x00 */
    char pad30[0x30];          /* 0x30 */
    struct Matrix4x3_local projMat;  /* 0x60 */
    char pad90[0x10];          /* 0x90 */
    struct Vector3 eye;        /* 0xa0 */
    struct Vector3 target;     /* 0xac */
    short angle;               /* 0xb8 */
};

extern void Camera_UpdateMatrices(struct Camera *self);

void func_ov006_020c1eb4(struct Camera *self)
{
    self->eye.x = 0;
    self->eye.y = 0xe000;
    self->eye.z = -0x22f00;
    self->target.x = 0;
    self->target.y = 0xf000;
    self->target.z = 0x32000;
    self->angle = 0xb00;
    Camera_UpdateMatrices(self);
}
