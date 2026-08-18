//cpp
// @symbol _ZN8CapEnemy6AddCapEj
/* recovered: named members + shared header, real C++ method
 *
 * Gives this enemy a cap: picks the model, loads it, binds it, and clears the
 * marker bits.
 *
 * The parameter is a SIX-way selector folded into two banks of three. `% 3`
 * picks the model, `>= 3` picks the bank (unk_110), and anything >= 6 means
 * "no cap" -- written as mCapId = 6, the same sentinel a failed SetFile falls
 * back to. unk_112 latches to 1 the first time a bank-1 cap is added and is
 * never cleared.
 *
 * The failure path is worth noting: on a SetFile failure it sets mCapId = 6
 * and then releases `data_ov002_020ff028[6]` -- index SIX of a six-entry table, one
 * past the end. That is the ROM's own behaviour, reproduced as-is.
 */
#include "CapEnemy.h"

extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *this_, void *file, int a, int b);
extern int _ZN13SharedFilePtr7ReleaseEv(void *sfp);
extern void func_ov001_020ab228(char *c, char *a1, int idx, int a3, int a5);
extern int func_02005e28(unsigned char *self);
extern void *data_ov002_020ff028[];
}

int CapEnemy::AddCap(unsigned int param)
{
    unk_111 = 0;
    if (param >= 6) {
        mCapId = 6;
        return 6;
    }

    mCapId = (int)param % 3;

    if (param >= 3) {
        unk_110 = 1;
        if (unk_112 == 0) unk_112 = 1;
    } else {
        unk_110 = 0;
    }

    void *file = _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff028[mCapId]);
    int ok = _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel, file, 1, -1);
    if (ok == 0) {
        mCapId = 6;
        return _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff028[mCapId]);
    }

    *(int *)(((long long)(int)((char *)this + 0xb0))) &= ~1;
    func_ov001_020ab228((char *)&mCapIcon, (char *)this, mCapId, unk_112, unk_110 != 0);

    int result = func_02005e28((unsigned char *)this);
    if (result != 0) {
        unsigned char *p = (unsigned char *)(((long long)(int)((char *)this + 0x113)));
        result = *p | 0x80;
        *p = result;
    }
    return result;
}
