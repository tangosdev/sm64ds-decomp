//cpp
// @symbol dCamera_c_classInit
/* Allocation factory historically labelled _ZN6CameraC1Ev. Its first action
 * overwrites r0 with fBase_c::operator new(sizeof(Camera)); a real constructor
 * must consume the incoming storage pointer. The factory table at 0x02086d78
 * independently confirms this is a maker function, not a C1 ABI variant.
 *
 * The typed hierarchy exposes the otherwise hand-written offsets: View's
 * matrix is the object at +0x50, and the three vptr transitions are dBase_c,
 * View, then Camera. The allocation fold itself remains explicit because the
 * pinned compiler rejects the required custom/placement operator-new source. */
#include "Camera.h"

extern "C" {
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN7fBase_cC2Ev(void *self);
void Matrix4x3_LoadIdentity(Matrix4x3 *matrix);
extern void *_ZTV7dBase_c[];
extern void *_ZTV4View[];
extern void *_ZTV6Camera[];
}

/* Reconstructed source-style name: SM64DS proves dCamera_c through RTTI,
 * allocation size, vtable identity, and the CAMERA registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Camera_Spawn. */
extern "C" Camera *dCamera_c_classInit()
{
    Camera *camera = (Camera *)_ZN7fBase_cnwEj(sizeof(Camera));
    if (camera) {
        _ZN7fBase_cC2Ev(camera);
        *(void ***)camera = _ZTV7dBase_c;
        *(void ***)camera = _ZTV4View;
        Matrix4x3_LoadIdentity(&camera->viewMat);
        *(void ***)camera = _ZTV6Camera;
    }
    return camera;
}
