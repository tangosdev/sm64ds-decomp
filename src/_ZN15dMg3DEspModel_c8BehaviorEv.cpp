//cpp
#include "dMg3DEspModel_c.h"

extern "C" {
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int, unsigned int, int, int, int, const void*, void*);
extern dMg3DEspModel_c::State data_ov006_0213c704;
}

// @symbol _ZN15dMg3DEspModel_c8BehaviorEv
void dMg3DEspModel_c::Behavior()
{
    /* mwcc's member-pointer null representation makes the second word
     * irrelevant when the function word is null. Spell that comparison out
     * to preserve the original early-return shape. */
    char* c = (char*)this;
    s32* state = (s32*)((int)c + 0x210);
    s32* idleState = (s32*)&data_ov006_0213c704;
    if (state[0] == idleState[0]) {
        if (state[1] == idleState[1]) return;
        if (*(s32*)(c + 0x210) == 0) return;
    }

    (this->*mState)();

    mModelAnim.Advance();
    mAnimSet.Behavior();

    if (unk_208 == 0) return;

    unk_204 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        unk_204, 0xec, 0x48c000, 0x140000, 0x200000, 0, 0);
}
