//cpp
// @symbol _ZN12dScMg3DEsp_c6RenderEv
#include "decl_common.h"
#include "dMg3DEspModel_c.h"
#include "dScMg3DEsp_c.h"
#include "TextureTransformer.h"
/* dScMg3DEsp_c::Render, vtable slot 9 (tools/rtti_vtables.py --own
 * dScMg3DEsp_c). Calls four ov006 helpers, resets the camera, then draws the
 * first model, the dMg3DEspModel_c block and the second model. */
extern "C" void Camera_UpdateMatrices(void *self);

s32 dScMg3DEsp_c::Render()
{
    func_ov006_020e81a4((char *)this);
    func_ov006_020e8e10((char *)this);
    func_ov006_020e8b18((char *)this);
    func_ov006_020e8354((char *)this);
    mCameraEyeX = 0;
    mCameraEyeY = 0xd0000;
    mCameraEyeZ = 0x40000;
    mCameraTargetX = 0xffed3000;
    mCameraTargetY = 0xe0000;
    mCameraTargetZ = 0x40000;
    mCameraAngle = 0xc00;
    Camera_UpdateMatrices(pad_4660);
    /* Model::data, at +8. Naming the member makes mwcc add the 8 in a separate instruction. */
    ((TextureTransformer *)mTextureTransformer)->Update(*(ModelComponents *)(mModel1 + 8));
    ((Model *)mModel1)->Render(0);
    ((dMg3DEspModel_c *)pad_4fd8)->Render();
    ((Model *)mModel2)->Render(0);
    return 1;
}
