//cpp
#include "BowserPuzzlePiece.h"

typedef int Fix12i;

typedef void (BowserPuzzlePiece::*PMF)();
struct Entry { PMF pmf; };
extern Entry data_ov064_0211c904[];

extern "C" void func_ov064_02118ee4(void* c);
extern "C" char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern "C" void func_ov064_02119010(void* c);
extern "C" int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* c, Fix12i a, Fix12i b);
extern "C" void func_ov064_02118fa4(void* c);

int BowserPuzzlePiece::Behavior() {
    func_ov064_02118ee4(this);
    (this->*data_ov064_0211c904[mState].pmf)();
    char* cc = (char*)this;
    char* p = 0;
    unsigned int id = mOtherPieceId;
    if (id != 0)
        p = _ZN8dActor_c10FindWithIDEj(id);
    if (p == 0 || *(unsigned char*)(p + 0xd6) == 0) {
        u16* ctr = &mMoveTimer;
        *ctr = *ctr + 1;
    }
    func_ov064_02119010(cc);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(cc, 0, 0) != 0)
        func_ov064_02118fa4(cc);
    return 1;
}
