//cpp
// @symbol _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player
/* recovered: real C++ method -- the compiler mangles the name
 *
 * Both reference parameters are genuinely unused by the ROM body; only `this`
 * reaches the code, and it reaches it as a plain word (stored into a global and
 * compared unsigned against a count), which is why the casts below are explicit.
 * They are not a reinterpretation -- the hand-spelled free function this replaces
 * did exactly the same thing to its first argument.
 */
#include "dEnemyBase_c.h"

extern "C" {
struct Obj;
extern struct Obj *data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc1c(void);
extern void func_ov004_020adc00(int v);
}

void dEnemyBase_c::KillByInvincibleChar(const Vector3_16 &, Player &)
{
    int thiz = (int)this;
    char *g = (char *)data_ov004_020beb68;
    if (g == 0) return;
    g += 0x4000;
    *(int *)(g + 0x654) = thiz;
    if (data_0209b308[2] == 5) {
        if (thiz == 0) return;
        if (func_ov004_020adc1c() != 0) {
            if ((unsigned)thiz >= (unsigned)func_ov004_020adc1c()) return;
        }
        func_ov004_020adc00(thiz);
        *((char *)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
        return;
    }
    if ((unsigned)thiz <= (unsigned)func_ov004_020adc1c()) return;
    func_ov004_020adc00(thiz);
    *((char *)data_ov004_020beb68 + 0x4000 + 0x65c) = 1;
}
