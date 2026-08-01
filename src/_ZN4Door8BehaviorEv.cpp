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
extern int func_ov100_02145370(char *c);

int Door::Behavior()
{
    int res = func_ov100_02145370(((char *)this));
    CallbackNode *node = *(CallbackNode**)((char*)&unk_110);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)((char *)this);
        (base->*(node->callback))(res);
    }
    return 1;
}
