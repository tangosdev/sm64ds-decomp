//cpp
// @symbol _ZN17daObjWlPolelift_c13InitResourcesEv
/* Load the lift model, bind its path/collision state, and initialize the
 * actor-owned shadow and cylinder through the reconstructed class layout. */
#include "daObjWlPolelift_c.h"
#include "SharedFilePtr.h"

extern "C" {
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset,
    Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags);
void func_ov026_021112a4(daObjWlPolelift_c *actor);
}

extern u32 data_0209caa0[];
extern SharedFilePtr data_ov026_02113ea0;
extern Vector3 data_ov026_02113a9c;

s32 daObjWlPolelift_c::InitResources()
{
    model.SetFile((BMD_File *)Model::LoadFile(data_ov026_02113ea0), 1, -1);
    if ((data_0209caa0[1] & 0x204) == 0)
        return 0;

    shadow.InitCuboid();
    path.FromID(param1 & 0xff);
    numNodes = path.NumNodes();
    moveSpeed = 1;

    Vector3 offset;
    offset.x = data_ov026_02113a9c.x;
    offset.y = data_ov026_02113a9c.y;
    offset.z = data_ov026_02113a9c.z;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &clsn, this, &offset, 0x50000, 0x12c000, 0x80000c, 0);
    func_ov026_021112a4(this);
    return 1;
}
