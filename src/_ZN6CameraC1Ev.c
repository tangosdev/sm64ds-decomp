#include "types.h"
// @symbol _ZN6CameraC1Ev
/* recovered: named members + shared header */
#include "Camera.h"
/* 4x3 matrix: 48 bytes */
typedef struct { s32 m[12]; } Matrix4x3;

typedef struct {
    u32* vtable;
    char pad4[0x4b];
    Matrix4x3 mat; /* offset 0x50 */
} Camera;

extern u32 data_0208e4b8;
extern u32 _ZTV4View;
extern u32 _ZTV6Camera;

extern Camera* _ZN7fBase_cnwEj(u32 size);
extern void _ZN7fBase_cC2Ev(Camera* this);
extern void Matrix4x3_LoadIdentity(Matrix4x3* mat);

Camera* _ZN6CameraC1Ev(Camera* this) {
    this = _ZN7fBase_cnwEj(0x1a8);
    if (this != 0) {
        _ZN7fBase_cC2Ev(this);
        this->vtable = &data_0208e4b8;
        this->vtable = &_ZTV4View;
        Matrix4x3_LoadIdentity(&this->mat);
        this->vtable = &_ZTV6Camera;
    }
    return this;
}
