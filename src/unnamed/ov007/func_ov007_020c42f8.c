#include "Ov007Sprite.h"
#include "math/Matrix.h"

extern void Matrix3x3_LoadIdentity(Matrix3x3 *m);
extern void func_ov007_020c3abc(Matrix3x3 *out, int ax, int ay, int az);
extern void G3_MultTransMtx33(int *a, int *v);

/* Loads the sprite's transform: translate to pos + posOffset, rotate by
 * rotX/rotY/rotZ, then scale by scaleX/scaleY (z stays 1.0). */
void func_ov007_020c42f8(Ov007Sprite *sprite) {
    int vec[3];
    Matrix3x3 mtx;
    int a, b;
    vec[0] = sprite->pos.x + sprite->posOffset.x;
    vec[1] = sprite->pos.y + sprite->posOffset.y;
    vec[2] = sprite->pos.z + sprite->posOffset.z;
    Matrix3x3_LoadIdentity(&mtx);
    func_ov007_020c3abc(&mtx, sprite->rotX, sprite->rotY, sprite->rotZ);
    G3_MultTransMtx33((int *)&mtx, vec);
    a = sprite->scaleX;
    b = sprite->scaleY;
    *(volatile int *)0x400046c = a;
    *(volatile int *)0x400046c = b;
    *(volatile int *)0x400046c = 0x1000;
}
