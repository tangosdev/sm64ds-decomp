/* HOST COPY of src/actors/daBombking_c.cpp -- King Bob-omb per-frame helper that
 * checks the health of the Player it is holding.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * Player::GetHealth reads `this` at +0x6d8 (mPlayerNo) to index the health
 * table, so it needs a real Player. Here the Player is the held one, stored at
 * `c->field_494`; the ROM loads it into r0 right before the call:
 *
 *     02123f30: ldr  r0, [r4, #0x494]   ; r0 = c->field_494  (the held Player)
 *     02123f34: cmp  r0, #0
 *     02123f38: beq  0x2123f88
 *     02123f3c: bl   Player::GetHealth  ; r0 = c->field_494
 *
 * (r4 is `c`.) The matched C spells the call as GetHealth() with no argument;
 * byte-identical on ARM because the held Player is already in r0 from the
 * `c->field_494 == 0` guard immediately above it. NOTE this differs from the
 * OTHER GetHealth caller, Player::St_Swim_Main, where the receiver is the
 * enclosing `this`; the register was proven per site, not assumed.
 *
 * On the host the GetHealth bridge (hal/player_bridges.cpp) is
 * `_ZN6Player9GetHealthEv(void *self)`; the zero-argument call pushes nothing,
 * so `self` is garbage and GetHealth indexes the health table with garbage
 * mPlayerNo. THE FIX passes the held Player explicitly -- c->field_494, the
 * ROM's r0.
 *
 * src/actors/daBombking_c.cpp is dropped from slice_gate32.txt in favour of this
 * file; the byte-locked source is unchanged.
 */

typedef int Fix12i;
struct C;
extern "C" int func_ov078_02123804(C* c);
extern "C" int _ZN6Player9GetHealthEv(void* self);   /* member: this off stack */
extern "C" void func_ov078_02123864(C* c);
extern "C" void func_ov002_020db54c(int self, int a, int b, int d);
extern "C" int _ZN9Animation8FinishedEv(void* anim);
extern "C" void KingBobOmb_SetState(C* c, void* p);
extern char data_ov078_0212709c[];

struct C {
    char pad0[0x8e];
    short field_8e;
    char pad90[0x31c-0x90];
    char anim_31c;
    char pad31d[0x430-0x31d];
    int field_430;
    char pad434[0x494-0x434];
    int field_494;
};

// PORT_HOST_ABI: implicit-register-arg (GetHealth receiver = c->field_494, rode r0 on ARM).
extern "C" int func_ov078_02123f1c(C* c)
{
    if (func_ov078_02123804(c) != 1) {
        if (c->field_494 == 0) goto L6c;
        if (_ZN6Player9GetHealthEv((void*)c->field_494) != 0) goto L6c;  /* <-- held Player, ROM's r0 */
    }
    func_ov078_02123864(c);
    if (c->field_494 != 0) {
        func_ov002_020db54c(c->field_494, 0, 0x50000, c->field_8e);
        c->field_430 = c->field_494;
        c->field_494 = 0;
    }
    return 1;
L6c:
    if (_ZN9Animation8FinishedEv(&c->anim_31c)) {
        KingBobOmb_SetState(c, data_ov078_0212709c);
    }
    return 1;
}
