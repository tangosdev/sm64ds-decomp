//cpp
// @symbol _ZN9TowerStep8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "TowerStep.h"
extern int DecIfAbove0_Byte(void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned,unsigned,unsigned,void*,unsigned);
extern int _ZN8Platform21UpdateModelPosAndRotYEv(void*);
extern int func_020393a4(int*,int);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void*,int,int);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void*);

int TowerStep::Behavior()
{
  if (DecIfAbove0_Byte((char*)&unk_31e) == 0) {
    short* p = (short*)(((int)((char*)this) + 0x94));
    *p = *p + 0x100;
    unk_08e = unk_094;
    unk_320 = _ZN5Sound8PlayLongEjjjRK7Vector3j(unk_320, 3, 0x88, (void*)((char*)&unk_074), 0);
    if ((unk_094 & 0x7fff) == 0) unk_31e = 0x3c;
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
  func_020393a4((int*)((char*)&mMeshCollider), 0x320000);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x320000, 0))
    _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
  return 1;
}
