//cpp
/* Player::St_Grabbed_Cleanup — clear flag bit1 at +0x2ec; if a held actor
 * (+0x35c) exists with actorID 0xbf, drop it and clear the slot. Returns 1.
 * Callee: _ZN6Player9DropActorEv (called with the held actor as arg).
 */
extern "C" {

typedef unsigned int u32;
typedef unsigned short u16;

struct Actor {
    char _pad0[0xc];
    u16 actorID;          /* 0xc */
};

struct Player {
    char _pad0[0x2ec];
    u32 unk2EC;           /* 0x2ec */
    char _pad1[0x35c - 0x2f0];
    struct Actor* held;   /* 0x35c */
};

extern void _ZN6Player9DropActorEv(struct Actor* a);

int _ZN6Player18St_Grabbed_CleanupEv(struct Player* this_)
{
    struct Actor* a;
    *(u32*)(((long long)(int)((char*)this_ + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
    a = this_->held;
    if (a) {
        int isBob = a->actorID == 0xbf;
        if (isBob) {
            _ZN6Player9DropActorEv(a);
            this_->held = 0;
        }
    }
    return 1;
}

}
