#include "Ov007Sprite.h"

typedef struct Mat3 { int m[9]; } Mat3;

extern void Matrix3x3_LoadIdentity(Mat3 *m);
extern void func_ov007_020c3abc(Mat3 *out, int ax, int ay, int az);
extern void func_0205532c(int *a, int *v);

/* Loads the sprite's transform: translate to pos + posOffset, rotate by
 * rotX/rotY/rotZ, then scale by scaleX/scaleY (z stays 1.0). */
void func_ov007_020c42f8(char *self);
void func_ov007_020c42f8(char *self) {
    Ov007Sprite *sprite = (Ov007Sprite *)self;
    int vec[3];
    Mat3 mtx;
    int a, b;
    vec[0] = sprite->pos.x + sprite->posOffset.x;
    vec[1] = sprite->pos.y + sprite->posOffset.y;
    vec[2] = sprite->pos.z + sprite->posOffset.z;
    Matrix3x3_LoadIdentity(&mtx);
    func_ov007_020c3abc(&mtx, sprite->rotX, sprite->rotY, sprite->rotZ);
    func_0205532c((int *)&mtx, vec);
    a = sprite->scaleX;
    b = sprite->scaleY;
    *(volatile int *)0x400046c = a;
    *(volatile int *)0x400046c = b;
    *(volatile int *)0x400046c = 0x1000;
}
