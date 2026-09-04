/* HOST COPY of src/func_ov002_020b3788.cpp -- daObjBlockL_c::OnAttacked2
 * (BigBrickBlock vtable slot 23), the handler a bro's PUNCH reaches on a brick
 * block.
 *
 * WHY THIS FILE EXISTS. This function's job, once the block variant/attacker
 * checks pass, is to dispatch its OWN vtable slot 31 (Kill) so the block
 * breaks. The byte-locked src models the vtable as a plain array of function
 * pointers taking self explicitly:
 *
 *     (*(struct Vt**)c)->fn(c)      // fn at vtable+0x7c == slot 31, int(*)(void*)
 *
 * which MSVC compiles __cdecl: c is PUSHED, the vtable word is loaded into a
 * register (not `this` in ecx), `call [reg+0x7c]` runs. Slot 31 (Kill) is
 * seated in this port __thiscall with no stack argument (bbb_kill ->
 * func_ov002_020b38a0, receiver in ecx). One vtable word cannot satisfy both:
 * MSVC loads the vtable pointer into ecx to reach the slot, so ecx holds the
 * VTABLE, not the object, when the call lands; bbb_kill reads `this` out of
 * ecx and Kill would run against _ZTV13BigBrickBlock and read the block's
 * fields out of vtable words -- a wrong-receiver call that faults.
 *
 * Raised inside the Player's own St_PunchKick attack callback
 * (func_ov002_020d8a50 -> func_ov002_020ef070 -> slot 23), that fault trips
 * the actor-walk quarantine net and soft-locks the bro exactly the way the
 * Yoshi eat-cap face did. Same shape, same fix as slot 22 in
 * unmatched/ArrowSign_OnAttacked1.cpp and the slot-23 DISPATCHER in
 * unmatched/Actor_OnAttacked2Dispatch.cpp.
 *
 * This copy is the matched source line for line with exactly ONE thing
 * changed: the slot-31 dispatch goes through a C++ virtual so MSVC emits the
 * same __thiscall bbb_kill expects (receiver in ecx, no push, no caller
 * cleanup). Every constant, offset and branch is the byte-locked original's.
 * src/ is untouched; slice_gate16.txt carries this file in place of the .cpp
 * and hal/actor_classes.cpp seats slot 23 on it (bbb_atk2).
 *
 * WHAT IT IS IN THE GAME. The star-tracking variant (id 0x11, actor 17) only
 * breaks when the attacker's +8 word is 2; every other variant (id 0xf etc.)
 * breaks on any punch. The check is the byte-locked original's, unchanged.
 */
#include "types.h"

struct BlockL {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30();
    virtual void Kill();   /* slot 31, vtable + 0x7c */
};

extern "C" void func_ov002_020b3788(char *c, char *arg1)
{
    BlockL *self = (BlockL *)c;
    int b = (*(unsigned short *)(c + 0xc) == 0x11);
    if (b) {
        if (*(int *)(arg1 + 8) != 2) return;
        self->Kill();
        return;
    }
    self->Kill();
}
