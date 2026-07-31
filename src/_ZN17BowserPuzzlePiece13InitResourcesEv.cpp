//cpp
// @symbol _ZN17BowserPuzzlePiece13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserPuzzlePiece.h"
typedef int Fix12i;
struct SharedFilePtr;
struct Actor;
struct PMF;
namespace Model { void LoadFile(SharedFilePtr& f); }
extern "C" int IsStarCollected(int r0, int r1);
struct MovingCylinderClsn { void Init(Actor* a, Fix12i b, int c, unsigned int d, unsigned int e); };

extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

int BowserPuzzlePiece::InitResources()
{
    Model::LoadFile(data_ov002_0210da10);
    Model::LoadFile(data_ov002_0210d9a8);
    unk_314 = (unk_008 >> 0xc) & 0xf;
    unk_318 = unk_008 & 1;
    if ((unk_008 & 0xf) > 1) unk_318 = 0;
    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0)) {
        return 0;
    }
    ((MovingCylinderClsn*)((char*)&mMovingCylinderClsn))->Init((Actor*)((char*)this), 0xc8000, 0x190000, 0x800004, 0);
    func_ov064_0211982c(((char*)this), &data_ov064_0211c934);
    return 1;
}
