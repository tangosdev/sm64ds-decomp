//cpp
// @symbol func_ov002_020aea30
/* THIS is the function `_ZN12dEnemyBase_c12KillByAttackER8dActor_c` was coined for,
   2026-08-27. That name sat on ov004 0x020aea30 -- the same address in the other
   overlay of this slot -- whose body walks an 8-byte table to an 0xffff sentinel:
   r0 there is a table base, not an actor. It is `func_ov004_020aea30` now.
   All 34 enemy-overlay call sites reach THIS body.

   The placeholder stays until somebody names it on evidence rather than by
   inheriting a name coined against the wrong body -- see
   notes/symbol-name-provenance.md. What IS settled is the shape:

     - FOUR arguments, not three. Every external call site materialises r3, and
       an unused fourth `int` parameter is what makes this file match; forwarding
       it to the pointer-to-member instead costs 0xc.
     - The first three, from the call sites: a dActor_c * attacker
       (dActor_c::ClosestPlayer, or the actor a collision search found), a
       dBgCh_Actr * collision object (daKrb_c passes &mWithMeshClsn at 0x1b4) or
       0, and a small int kind, 0..7.

   Callers set mDeathState (+0x10c) immediately before calling, and this reads it
   back to pick the handler, so it starts a death sequence rather than performing
   one. */
// If the state index at +0x10c is set: clears bit 0x10000000 at +0xb0, zeroes
// the halfword at +0x102, invokes the pointer-to-member-function from
// data_ov002_0210db80[index-1] (forwarding both int args), then writes
// -0x2000 to +0x9c and clears the +0xb0 bit again.
struct C;
typedef void (C::*PMF)(int, int);
extern PMF data_ov002_0210db80[];
struct C {
  char pad0[0x9c];
  int f9c;
  char pad1[0x10];
  int fb0;
  char pad2[0x4e];
  short f102;
  char pad3[8];
  int f10c;
};
extern "C" void func_ov002_020aea30(C* c, int a, int b, int d) {
  if (c->f10c == 0) return;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
  c->f102 = 0;
  (c->*data_ov002_0210db80[c->f10c - 1])(a, b);
  c->f9c = -0x2000;
  (*(unsigned int*)((char*)c + 0xb0)) &= ~0x10000000;
}
