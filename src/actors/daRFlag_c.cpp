//cpp
/* ov062/daRFlag_c. The race flag in Koopa the Quick's course.
 *
 * Cartridge names: _ZTS9daRFlag_c at 0x0211dc18, _ZTI9daRFlag_c at 0x0211dc24,
 * vtable address point _ZTV9daRFlag_c at 0x0211dc54. Eight functions,
 * 0x0211af38..0x0211b248. The next object is daJango_c at 0x0211b248.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. defer_codegen off makes CodeWarrior emit
 * each definition where it stands, and the destructor pair comes out D1 then
 * D0 with no enrolled D2.
 */

#include "daRFlag_c.h"
#include "decl_common.h"
#include "decl_Timer.h"
#include "SharedFilePtr.h"

struct BMD_File;
struct BCA_File;

extern "C" {
void _ZN5Timer9StopTimerEv(void *);
int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *, int, int, unsigned int);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c *, int, int, unsigned int, unsigned int);
void *_ZN7fBase_cnwEj(unsigned);
void _ZN8dActor_cC2Ev(void *);
void _ZN7dCcAc_cC1Ev(void *);
void _ZN9ModelAnimC1Ev(void *);
void Matrix4x3_FromRotationY(void *, int);
extern char data_0209d4c8[];
}

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- D1 0x0211af38 size 0x38, D0 0x0211af70 size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daRFlag_cD1Ev
// @symbol _ZN9daRFlag_cD0Ev
/* Empty body: the vptr store and the member destructors (dCcAc_c at 0xd4,
 * ModelAnim at 0x108, then dActor_c) are consequences of the class. D0's
 * deallocation is the inline operator delete on dActor_c. */
daRFlag_c::~daRFlag_c()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov062_0211afbc, 0x0211afbc, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov062_0211afbc
/* Places the model matrix from the body angle and the position. */
extern "C" void func_ov062_0211afbc(char *t)
{
    Matrix4x3_FromRotationY(t + 0x124, *(short *)(t + 0x8e));
    *(int *)(t + 0x148) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x14c) = (*(int *)(t + 0x60) + 0x3c000) >> 3;
    *(int *)(t + 0x150) = *(int *)(t + 0x64) >> 3;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN9daRFlag_c16CleanupResourcesEv, 0x0211b000, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daRFlag_c16CleanupResourcesEv
int daRFlag_c::CleanupResources()
{
    data_ov062_0211e0d4.Release();
    data_ov062_0211e0dc.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN9daRFlag_c6RenderEv, 0x0211b030, size 0x2c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daRFlag_c6RenderEv
int daRFlag_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN9daRFlag_c8BehaviorEv, 0x0211b05c, size 0x10c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daRFlag_c8BehaviorEv
int daRFlag_c::Behavior()
{
    unsigned int id;
    dActor_c *a;
    int b;

    if (mHasTouchedFlag == 0) {
        id = mdCcAc_c.otherOwner;
        if (id != 0) {
            a = dActor_c::FindWithID(id);
            if (a != 0) {
                b = a->actorID;
                b = b == 0xBF;
                if (b) {
                    mHasTouchedFlag = 1;
                    _ZN5Timer9StopTimerEv(data_0209d4c8);
                    mVictoryTimer = 1;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (mVictoryTimer != 0) {
        *(unsigned short *)(((int)((char *)this) + 0x16C)) += 1;
        if (mVictoryTimer >= 0x5A) {
            if (_ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x7F, 0, 0x8777, 0) != 0) {
                mVictoryTimer = 0;
            }
        }
    }

    mModelAnim.Advance();
    func_ov062_0211afbc(((char *)this));
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN9daRFlag_c13InitResourcesEv, 0x0211b168, size 0xa0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9daRFlag_c13InitResourcesEv
/* SetAnim and dCcAc_c::Init are declared with the loose spellings the call
 * sites were matched under. The mangled names pass Fix12 by value. */
int daRFlag_c::InitResources()
{
    mModelAnim.SetFile(
        (BMD_File *)Model::LoadFile(data_ov062_0211e0d4), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)Animation::LoadFile(data_ov062_0211e0dc),
        0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x35555, 0x294000, 0x280000c, 0);
    mHasTouchedFlag = 0xff;
    mVictoryTimer = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- daRFlag_c_classInit, 0x0211b208, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol daRFlag_c_classInit
extern "C" int *daRFlag_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(372);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)&_ZTV9daRFlag_c[2];
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
        _ZN9ModelAnimC1Ev((char *)p + 0x108);
    }
    return p;
}
