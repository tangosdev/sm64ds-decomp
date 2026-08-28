#include "types.h"
// @symbol _ZN6CameraC1Ev
/* Allocation factory historically labelled as Camera C1.
 * notes/ctor-migration.md section 5 proves this is not a source-expressible
 * constructor: it allocates, null-checks, and then performs construction. */
typedef struct { s32 m[12]; } CameraFactoryMatrix4x3;

typedef struct {
    u32 *vtable;
    char pad4[0x4b];
    CameraFactoryMatrix4x3 mat; /* offset 0x50 */
} CameraFactoryObject;

extern u32 data_0208e4b8;
extern u32 _ZTV4View;
extern u32 _ZTV6Camera;

extern CameraFactoryObject *_ZN7fBase_cnwEj(u32 size);
extern void _ZN7fBase_cC2Ev(CameraFactoryObject *self);
extern void Matrix4x3_LoadIdentity(CameraFactoryMatrix4x3 *mat);

CameraFactoryObject *_ZN6CameraC1Ev(CameraFactoryObject *self)
{
    self = _ZN7fBase_cnwEj(0x1a8);
    if (self != 0) {
        _ZN7fBase_cC2Ev(self);
        self->vtable = &data_0208e4b8;
        self->vtable = &_ZTV4View;
        Matrix4x3_LoadIdentity(&self->mat);
        self->vtable = &_ZTV6Camera;
    }
    return self;
}
