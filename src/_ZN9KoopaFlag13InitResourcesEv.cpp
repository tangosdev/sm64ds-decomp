//cpp
// @symbol _ZN9KoopaFlag13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "KoopaFlag.h"
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct dActor_c;

/* Model / ModelBase / Animation / ModelAnim are the real classes now, through
   this actor's header. */
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File* f, int a, Fix12i b, unsigned int c);

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *, dActor_c* a, Fix12i b, Fix12i c, unsigned int d, unsigned int e);


extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

int KoopaFlag::InitResources()
{
    mModelAnim.SetFile(
        (BMD_File*)Model::LoadFile(data_ov062_0211e0d4), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, (BCA_File*)Animation::LoadFile(data_ov062_0211e0dc), 0, 0x1000, 0);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x35555, 0x294000, 0x280000c, 0);
    mHasTouchedFlag = 0xff;
    mVictoryTimer = 0;
    return 1;
}
