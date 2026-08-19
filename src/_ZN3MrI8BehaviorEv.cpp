//cpp
// @symbol _ZN3MrI8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "MrI.h"
extern "C" {
    void func_ov071_021215c0(void *c);
    void func_0200f760(void *c, void *p);
    void func_ov071_02120c90(void *c);
}

struct dCc_c {
    void Clear();
    void Update();
};

int MrI::Behavior()
{
    func_ov071_021215c0(((char *)this));
    func_0200f760(((char *)this), ((char *)this) + 0x174);
    unk_20c = mAngleY;
    ((dCc_c*)((char *)&mdCcAcPos_c))->Clear();
    ((dCc_c*)((char *)&mdCcAcPos_c))->Update();
    func_ov071_02120c90(((char *)this));
    return 1;
}
