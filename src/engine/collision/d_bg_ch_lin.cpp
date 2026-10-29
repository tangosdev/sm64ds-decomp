//cpp
/* leftovers:
 * SetObjAndLine's marker stays above the definition: callers still declare
 * the unconverted Vec3* spelling, so the definition does not bind over the
 * plurality. GetClsnPos returns lineEnd through the canonical Vector3
 * value-return interface. The intact TU preserves its hidden-result ABI.
 * func_02037608 upcasts through a reference so the multiple-inheritance
 * adjustment is unconditional. func_ov002_020fea4c and func_ov002_020fea68
 * are GetEnd and GetStart. D0 and the dBgPi thunks are emitted from the
 * written destructor.
 */
/* Compiler-emitted below from the written bodies (D0, dBgPi thunks); also
 * SetObjAndLine -- its callers still declare unconverted Vec3* spellings,
 * so its mark stays up here and the definition does not bind over the
 * plurality.
// @symbol _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c
// @symbol _ZN9dBgCh_LinD0Ev
// @symbol _ZThn16_N9dBgCh_LinD0Ev
// @symbol _ZThn16_N9dBgCh_LinD1Ev
*/
/* dBgCh_Lin: line-segment collision query. dBgCh, a dBgPi hit at 0x10,
 * and a dM3dGLin at 0x38. Type name 0x02099290, vtable 0x020992a4, dBgPi
 * secondary vtable 0x020992b4. lineEnd starts as the segment end and
 * becomes the hit point. The constructor and destructor are defined first. */
#include "dBgCh_Lin.h"

extern "C" {
void func_ov002_020fea4c(int *a, int *b);       /* dM3dGLin::GetEnd */
void func_ov002_020fea68(int *a, int *b);       /* dM3dGLin::GetStart */
Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
}

// @symbol _ZN9dBgCh_LinC1Ev
dBgCh_Lin::dBgCh_Lin()
{
    lineEnd.z = 0;
    lineEnd.y = lineEnd.z;
    lineEnd.x = lineEnd.y;
    clsnDist = 0;
}

// @symbol _ZN9dBgCh_LinD1Ev
dBgCh_Lin::~dBgCh_Lin()
{
}

void dBgCh_Lin::SetObjAndLine(const Vector3 &start, const Vector3 &end,
                              dActor_c *actor)
{
    Vector3 midpoint;
    dM3dGLin::Set(start, end);
    SetActor(actor);
    func_02037608();
    midpoint.x = start.x + end.x;
    midpoint.y = start.y + end.y;
    midpoint.z = start.z + end.z;
    midpoint.x >>= 1;
    midpoint.y >>= 1;
    midpoint.z >>= 1;
    mBoundSphere.Set(midpoint, (clsnDist >> 1) + 0x1000);
}

// @symbol _ZN9dBgCh_Lin13func_02037608Ev
void dBgCh_Lin::func_02037608()
{
    Vector3 startPoint;
    Vector3 endPoint;
    hasClsn = 0;
    /* through the REFERENCE: a pointer-level upcast makes mwcc emit the
       null-checked MI adjustment (movs/addne), the ROM's is unconditional */
    func_ov002_020fea4c((int *)&endPoint, (int *)&(dM3dGLin &)*this);
    lineEnd = endPoint;
    func_ov002_020fea68((int *)&startPoint, (int *)&(dM3dGLin &)*this);
    clsnDist = Vec3_Dist(&lineEnd, &startPoint);
    dBgPi::Reset();
}

// @symbol _ZN9dBgCh_Lin10SetClsnPosERK7Vector3
void dBgCh_Lin::SetClsnPos(const Vector3 &pos)
{
    lineEnd = pos;
}

/* Returns the line endpoint, or the collision point after a hit has
   replaced lineEnd. */
// @symbol _ZN9dBgCh_Lin10GetClsnPosEv
Vector3 dBgCh_Lin::GetClsnPos()
{
    return lineEnd;
}
