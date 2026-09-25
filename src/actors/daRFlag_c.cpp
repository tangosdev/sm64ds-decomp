//cpp
/**
 * The flag at the end of Koopa the Quick's race.
 *
 * While armed (mHasTouchedFlag == 0), Mario touching the cylinder stops
 * the race timer and plays sound 0x1F/0x14. From then mVictoryTimer
 * counts frames; from frame 90 it retries sound 0x1F/0x7F each frame
 * until PlaySub accepts it. The model sits 60.0 above the actor.
 *
 * Source order is ROM-ascending: `#pragma defer_codegen off` makes mwcc
 * emit each definition where it stands, and the one destructor
 * definition emits retail D1 then D0 (no D2).
 *
 * The name daRFlag_c_classInit is coined from the RTTI class name; the
 * ROM has no symbol for the factory.
 *
 * Leftover: SetAnim and dCcAc_c::Init stay the mangled calls. Spelling
 *   them as methods changed this function's size (Fix12 by value).
 * Leftover: `b = (actorID == 0xBF)` has to be an int. A bool does not
 *   match.
 * Leftover: func_ov062_0211afbc keeps the linker name. The enrolled
 *   symbol is that spelling.
 */

#include "daRFlag_c.h"
#include "decl_common.h"
#include "Timer.h"
#include "SharedFilePtr.h"

struct BMD_File;
struct BCA_File;

extern "C" {

int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int, unsigned int, unsigned int, int, int);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File *, int, int, unsigned int);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c *, int, int, unsigned int, unsigned int);
void Matrix4x3_FromRotationY(void *, int);
extern Timer data_0209d4c8;
}

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

#pragma defer_codegen off

// @symbol _ZN9daRFlag_cD1Ev
// @symbol _ZN9daRFlag_cD0Ev
/* Empty body: the vptr store and the member destructors (dCcAc_c at 0xd4,
 * ModelAnim at 0x108, then dActor_c) are consequences of the class. D0's
 * deallocation is the inline operator delete on dActor_c. */
daRFlag_c::~daRFlag_c()
{
}

// @symbol func_ov062_0211afbc
/* Builds the model matrix from the actor's facing and position.
 * The flag graphic sits 60.0 (0x3c000) above mPosY. Model-matrix
 * translation is in 1/8 world units, so the position is shifted down by 3. */
extern "C" void func_ov062_0211afbc(char *raw)
{
    daRFlag_c *flag = (daRFlag_c *)raw;
    Matrix4x3_FromRotationY(&flag->mModelAnim.mat4x3, flag->mAngleY);
    flag->mModelAnim.mat4x3.t.x = flag->mPosX >> 3;
    flag->mModelAnim.mat4x3.t.y = (flag->mPosY + 0x3c000) >> 3;
    flag->mModelAnim.mat4x3.t.z = flag->mPosZ >> 3;
}

// @symbol _ZN9daRFlag_c16CleanupResourcesEv
int daRFlag_c::CleanupResources()
{
    data_ov062_0211e0d4.Release();
    data_ov062_0211e0dc.Release();
    return 1;
}

// @symbol _ZN9daRFlag_c6RenderEv
int daRFlag_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

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
                    data_0209d4c8.StopTimer();
                    mVictoryTimer = 1;
                    _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x1F, 0x14, 0x7F, 0x6B000, 0);
                }
            }
        }
    }

    if (mVictoryTimer != 0) {
        mVictoryTimer += 1;
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

// @symbol daRFlag_c_classInit
extern "C" daRFlag_c *daRFlag_c_classInit(void)
{
    return new daRFlag_c();
}
