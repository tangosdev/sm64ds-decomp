//cpp
// @symbol _ZN4Door8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Door.h"
struct Base {};
typedef void (Base::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern "C" {
extern int func_ov100_02145f00(char *c);
}

/* Calls func_ov100_02145f00, not func_ov100_02145370. The call is a relocation,
   which match.py compares as a wildcard, so the byte gate passed the wrong
   callee happily -- and the ROM link could not see it either until this file
   became enrollable. */
int Door::Behavior()
{
    int res = func_ov100_02145f00(((char *)this));
    CallbackNode *node = *(CallbackNode**)((char*)&unk_110);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)((char *)this);
        (base->*(node->callback))(res);
    }
    return 1;
}
