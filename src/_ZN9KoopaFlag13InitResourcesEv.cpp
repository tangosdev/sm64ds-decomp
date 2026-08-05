//cpp
// @symbol _ZN9KoopaFlag13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "KoopaFlag.h"
typedef int Fix12;
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct Actor;

struct Model {
    static void* LoadFile(SharedFilePtr& f);
};
struct ModelBase {
    void SetFile(BMD_File* f, int a, int b);
};
struct Animation {
    static void* LoadFile(SharedFilePtr& f);
};
struct ModelAnim {
    void SetAnim(BCA_File* f, int a, Fix12 b, unsigned int c);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *, BCA_File* f, int a, Fix12 b, unsigned int c);

struct MovingCylinderClsn {
    void Init(Actor* a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *, Actor* a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);


extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

int KoopaFlag::InitResources()
{
    ((ModelBase*)((char*)&mModelAnim))->SetFile(
        (BMD_File*)Model::LoadFile(data_ov062_0211e0d4), 1, -1);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((ModelAnim*)((char*)&mModelAnim), (BCA_File*)Animation::LoadFile(data_ov062_0211e0dc), 0, 0x1000, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj((MovingCylinderClsn*)((char*)&mMovingCylinderClsn), (Actor*)((char*)this), 0x35555, 0x294000, 0x280000c, 0);
    unk_16e = 0xff;
    mVictoryTimer = 0;
    return 1;
}
