//cpp
/**
 * Whomp's Fortress pole billboard.
 *
 * Scenery model on a pole. Direct base is dActor_c; the owned
 * member is Model at 0xd4, which closes the class on the factory's
 * literal 0x124.
 *
 * daObjBkBillboard_c_classInit is reconstructed (RTTI
 * daObjBkBillboard_c, BK_BILLBOARD registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: data_ov015_02114960 is this overlay's BMD SharedFilePtr
 *   handle (sinit constructs file ID 1416). This TU is text-only, so
 *   it stays an extern.
 * Leftover: g_profile_BK_BILLBOARD lives outside this TU (S14).
 * Leftover: func_ov015_02111214 keeps its C-ABI cartridge name; it is
 *   not a vtable slot.
 * Leftover: pad_0d0[4] is unread in this TU. dActor_c is 0xd0 and
 *   Model needs no more than 4-byte alignment, so those four bytes
 *   are an unknown field, not alignment of mModel.
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 */

#include "daObjBkBillboard_c.h"
#include "SharedFilePtr.h"

extern "C" {
extern void Matrix4x3_FromRotationY(void *, int);
extern SharedFilePtr data_ov015_02114960;
void func_ov015_02111214(daObjBkBillboard_c *t);
}

// @symbol daObjBkBillboard_c_classInit
extern "C" daObjBkBillboard_c *daObjBkBillboard_c_classInit()
{
    return new daObjBkBillboard_c();
}

// @symbol _ZN18daObjBkBillboard_c13InitResourcesEv
s32 daObjBkBillboard_c::InitResources()
{
    void *file = Model::LoadFile(data_ov015_02114960);
    mModel.SetFile((BMD_File *)file, 1, -1);
    func_ov015_02111214(this);
    return 1;
}

// @symbol _ZN18daObjBkBillboard_c6RenderEv
s32 daObjBkBillboard_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjBkBillboard_c16CleanupResourcesEv
s32 daObjBkBillboard_c::CleanupResources()
{
    data_ov015_02114960.Release();
    return 1;
}

// @symbol func_ov015_02111214
extern "C" void func_ov015_02111214(daObjBkBillboard_c *t)
{
    Matrix4x3_FromRotationY(&t->mModel.mat4x3, t->mAngleY);
    t->mModel.mat4x3.t.x = t->mPosX >> 3;
    t->mModel.mat4x3.t.y = t->mPosY >> 3;
    t->mModel.mat4x3.t.z = t->mPosZ >> 3;
}
