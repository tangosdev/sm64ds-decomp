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
struct MovingCylinderClsn {
    void Init(Actor* a, Fix12 b, Fix12 c, unsigned int d, unsigned int e);
};

extern SharedFilePtr data_ov062_0211e0d4;
extern SharedFilePtr data_ov062_0211e0dc;

int KoopaFlag::InitResources()
{
    ((ModelBase*)((char*)&mModelAnim))->SetFile(
        (BMD_File*)Model::LoadFile(data_ov062_0211e0d4), 1, -1);
    ((ModelAnim*)((char*)&mModelAnim))->SetAnim(
        (BCA_File*)Animation::LoadFile(data_ov062_0211e0dc), 0, 0x1000, 0);
    ((MovingCylinderClsn*)((char*)&mMovingCylinderClsn))->Init((Actor*)((char*)this), 0x35555, 0x294000, 0x280000c, 0);
    unk_16e = 0xff;
    mVictoryTimer = 0;
    return 1;
}
