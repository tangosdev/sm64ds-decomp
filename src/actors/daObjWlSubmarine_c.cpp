//cpp
/**
 * Wet-Dry World's submarine platform (WL_SUBMARINE).
 *
 * InitResources loads the model and collision files into the BSS
 * homes, sets them on the mesh collider, and enables it unless a
 * system state word vetoes; CleanupResources disables the collider
 * and releases both files; Render draws the model.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile keeps its mangled spelling (by-value
 *   Fix12<int> parameters, wall 6az).
 * Leftover: data_ov026_02112fd4 is the unnamed CLPS home this TU
 *   feeds SetFile; data_0209caa0[1] is the shared system state word
 *   whose 0x204 bit vetoes the enable.
 */

#include "daObjWlSubmarine_c.h"
#include "SharedFilePtr.h"

/* BSS file homes (symbols.txt); Init loads them, Cleanup releases them. */
extern SharedFilePtr Submarine_ModelFile;
extern SharedFilePtr Submarine_ClsnFile;

extern "C" {
extern char data_ov026_02112fd4[];
extern char data_0209caa0[];
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *, int, void *, int, int, void *);
}

/* Emission order is ROM order: the destructor pair must stay first.
 * Do not reorder. */
#pragma defer_codegen off

// @symbol _ZN18daObjWlSubmarine_cD1Ev
// @symbol _ZN18daObjWlSubmarine_cD0Ev
/* One out-of-line definition; mwccarm emits D1 and D0 from it, in that
 * order: its own vptr, then dBgActor_c's inlined, then dBgActor_c's
 * Model and dBgW_KcMbg, then dActor_c. */
daObjWlSubmarine_c::~daObjWlSubmarine_c()
{
}

// @symbol _ZN18daObjWlSubmarine_c16CleanupResourcesEv
s32 daObjWlSubmarine_c::CleanupResources() {
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    Submarine_ModelFile.Release();
    Submarine_ClsnFile.Release();
    return 1;
}

// @symbol _ZN18daObjWlSubmarine_c6RenderEv
s32 daObjWlSubmarine_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN18daObjWlSubmarine_c13InitResourcesEv
s32 daObjWlSubmarine_c::InitResources() {
  void *m = Model::LoadFile(Submarine_ModelFile);
  mModel.SetFile((BMD_File *)m, 1, -1);
  UpdateModelPosAndRotY();
  UpdateClsnPosAndRot();
  {
    void *k = dBgW_Kc::LoadFile(Submarine_ClsnFile);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,(int)k,&mClsnMat,0x199,mAngleY,(void*)data_ov026_02112fd4);
  }
  if(*(int*)(data_0209caa0+4) & 0x204) return 0;
  mMeshCollider.Enable(this);
  return 1;
}
