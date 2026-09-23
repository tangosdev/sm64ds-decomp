//cpp
/**
 * Wet-Dry World's Koopa shutter (WL_KOOPA_SHUTTER).
 *
 * InitResources loads the model and collision files into the BSS
 * homes, sets them on the mesh collider, and enables it unless a
 * system state word vetoes; CleanupResources disables the collider
 * and releases both files; Render draws the model.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: data_ov026_02112f74 is the unnamed CLPS home this TU
 *   feeds SetFile; data_0209caa0[1] is the shared system state word
 *   whose 0x204 bit vetoes the enable.
 */

#include "daObjWlKoopaShutter_c.h"
#include "SharedFilePtr.h"

/* BSS file homes (symbols.txt); Init loads them, Cleanup releases them. */
extern SharedFilePtr BowserShutter_ModelFile;
extern SharedFilePtr BowserShutter_ClsnFile;

extern "C" {
extern char data_ov026_02112f74[];
extern char data_0209caa0[];
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN21daObjWlKoopaShutter_cD1Ev
// @symbol _ZN21daObjWlKoopaShutter_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * Model and dBgW_KcMbg, then dActor_c. */
daObjWlKoopaShutter_c::~daObjWlKoopaShutter_c()
{
}

// @symbol _ZN21daObjWlKoopaShutter_c16CleanupResourcesEv
s32 daObjWlKoopaShutter_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    BowserShutter_ModelFile.Release();
    BowserShutter_ClsnFile.Release();
    return 1;
}

// @symbol _ZN21daObjWlKoopaShutter_c6RenderEv
s32 daObjWlKoopaShutter_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN21daObjWlKoopaShutter_c8BehaviorEv
s32 daObjWlKoopaShutter_c::Behavior() {
    return 1;
}

// @symbol _ZN21daObjWlKoopaShutter_c13InitResourcesEv
s32 daObjWlKoopaShutter_c::InitResources() {
  void *m = Model::LoadFile(BowserShutter_ModelFile);
  mModel.SetFile((BMD_File *)m, 1, -1);
  UpdateModelPosAndRotY();
  UpdateClsnPosAndRot();
  {
    void *k = dBgW_Kc::LoadFile(BowserShutter_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,(int)k,&mClsnMat,0x199,mAngleY,(void*)data_ov026_02112f74);
  }
  if(*(int*)(data_0209caa0+4) & 0x204) return 0;
  mMeshCollider.Enable(this);
  return 1;
}
