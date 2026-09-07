//cpp
// @symbol _ZN12dScMg3DEsp_c6RenderEv
#include "decl_common.h"
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
/* dScMg3DEsp_c::Render -- vtable slot 9.
 *
 * Attributed by tools/rtti_vtables.py --own dScMg3DEsp_c, this class's own slot 9
 * (dScMgBase_c::Render). The old file's `recovered name: dScMg3DEsp_c_Render` agreed. */
extern "C" void Camera_UpdateMatrices(char *c);

struct ModelComponents;
struct TextureTransformer { void Update(ModelComponents &m); };

struct Obj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void vcall(int); };

s32 dScMg3DEsp_c::Render()
{
    char *c = (char *)this;
    func_ov006_020e81a4(c);
    func_ov006_020e8e10(c);
    func_ov006_020e8b18(c);
    func_ov006_020e8354(c);
    this->mCameraEyeX = 0;
    this->mCameraEyeY = 0xd0000;
    this->mCameraEyeZ = 0x40000;
    this->mCameraTargetX = 0xffed3000;
    this->mCameraTargetY = 0xe0000;
    this->mCameraTargetZ = 0x40000;
    this->mCameraAngle = 0xc00;
    Camera_UpdateMatrices(c + 0x4660);
    ((TextureTransformer *)(c + 0x51f4))->Update(*(ModelComponents *)(c + 0x4f40));
    ((Obj *)(c + 0x4f38))->vcall(0);
    ((dMg3DEspModel_c*)(c + 0x4fd8))->Render();
    ((Obj *)(c + 0x4f88))->vcall(0);
    return 1;
}
