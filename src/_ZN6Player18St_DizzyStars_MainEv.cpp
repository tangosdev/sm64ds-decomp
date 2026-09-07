//cpp
// @symbol _ZN6Player18St_DizzyStars_MainEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
extern "C" {
extern int func_ov002_020d98b4(void*);
extern int data_ov002_0211013c[];   /* the walk state */
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
}

int Player::St_DizzyStars_Main()
{
    func_ov002_020d98b4(this);
    /* Read SIGNED on purpose. mStateTimer is u16 and every other user treats
       it that way, but this test is `> 0` against a signed short, which is a
       bgt rather than a bhi. Spelling it unsigned changes the instruction. */
    if (*(s16*)&mStateTimer > 0) return 1;
    _ZN6Player11ChangeStateERNS_5StateE(this, data_ov002_0211013c);
    return 1;
}
