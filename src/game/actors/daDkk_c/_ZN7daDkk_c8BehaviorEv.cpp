//cpp
// @symbol _ZN7daDkk_c8BehaviorEv
/* func_ov025_02111b64, 0xc0 bytes -- vtable slot 6, override of the
 * abstract slot daDsnBase_c leaves null.
 *
 * Switches on mState (0x398, this class's own) to one of eight per-state
 * step functions (five shared with Thwomp in ov091, three private to this
 * overlay: func_ov025_02111a84/021119f4/021119a4, left as free functions --
 * out of scope for this pass), then runs the shared post-step housekeeping
 * every daDsnBase_c leaf needs.
 */
#include "daDkk_c.h"

typedef int Fix12i;
extern "C" {
extern void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void* p);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void* p, Fix12i a, Fix12i b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void* p);
extern void func_ov091_02133020(void* c);
extern void func_ov091_02132ff4(void* c);
extern void func_ov091_02132f04(void* c);
extern void func_ov091_02132e98(void* c);
extern void func_ov091_02132e64(void* c);
extern void func_ov025_02111a84(void* c);
extern void func_ov025_021119f4(void* c);
extern void func_ov025_021119a4(void* c);
extern void func_ov091_02133098(void* c);
extern int func_ov091_02132dc0(void* c);
}

int daDkk_c::Behavior()
{
    char *c = (char *)this;
    switch (mState) {
    case 0: func_ov091_02133020(c); break;
    case 1: func_ov091_02132ff4(c); break;
    case 2: func_ov091_02132f04(c); break;
    case 3: func_ov091_02132e98(c); break;
    case 4: func_ov091_02132e64(c); break;
    case 5: func_ov025_02111a84(c); break;
    case 6: func_ov025_021119f4(c); break;
    case 7: func_ov025_021119a4(c); break;
    }
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(c);
    func_ov091_02133098(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0 ||
        func_ov091_02132dc0(c) != 0) {
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    }
    return 1;
}
