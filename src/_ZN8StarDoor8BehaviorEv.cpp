//cpp
// @symbol _ZN8StarDoor8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "StarDoor.h"
struct Base {};
typedef void (Base::*PMF)(int);
struct CallbackNode {
    char pad[8];
    PMF callback;
};
extern "C" {
extern int func_ov100_02145f00(char *c);
}

/* Calls func_ov100_02145f00, not func_ov100_02145370. The call is a
   relocation in our object, which match.py compares as a wildcard, so the
   byte gate passed the wrong callee happily. The ROM link could not see it
   either: the declaration above mangled without C linkage, the reference
   went unresolvable, and eligible.py refused to enroll the file -- so it
   never reached the link that would have caught it.

   Verified against the ROM: the single BL at 0x021460e4 is 0xebffff85,
   which targets 0x02145f00. */

int StarDoor::Behavior()
{
    int res = func_ov100_02145f00(((char *)this));
    CallbackNode *node = *(CallbackNode**)((char*)&unk_110);
    if (*(int*)&node->callback != 0) {
        Base *base = (Base*)((char *)this);
        (base->*(node->callback))(res);
    }
    return 1;
}
