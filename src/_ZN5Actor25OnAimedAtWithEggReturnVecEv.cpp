//cpp
/* Actor::OnAimedAtWithEggReturnVec() at 0x020100dc, 0x48 bytes -- vtable slot 30.
 *
 * Where an egg should aim: the actor's own position, raised by the lock-on
 * radius slot 29 reports. The two slots are a pair -- 29 answers "how far out
 * will an egg home on me", 30 answers "at what point" -- and 30 asks 29
 * VIRTUALLY, so a leaf class that widens its radius moves its aim point for
 * free. That call is now spelled `self->OnAimedAtWithEgg()` and the compiler
 * emits the same `ldr r1,[r0]; ldr r1,[r1,#0x74]; blx r1` the ROM has, because
 * 0x74/4 = 29 falls out of include/Actor.h's declaration order.
 *
 * IT RETURNS A Vector3 BY VALUE, and include/Actor.h now says so -- it said
 * `virtual int` until this file was migrated. The ROM is unambiguous: r0 is
 * written and never read as an input, r1 supplies every field load, and the
 * three stores go through r0 at +0/+4/+8. That is AAPCS indirect return, with
 * `this` displaced into r1. Nothing in the tree calls slot 30, so the wrong
 * type had never been falsifiable; migrating the definition is what exposed it.
 *
 * WHY THIS IS STILL AN extern "C" FREE FUNCTION AND NOT A METHOD. The honest
 * spelling
 *
 *     Vector3 Actor::OnAimedAtWithEggReturnVec()
 *     { Vector3 aim; aim.x = mPosX; ...; aim.y += OnAimedAtWithEgg(); return aim; }
 *
 * was written and measured first. mwcc 2004/b56 does not apply the named return
 * value optimization here: it builds the vector on the stack (`sub sp,#0x10`,
 * stores to `[sp]`) and copies it into the caller's slot at the end with
 * `ldm r1,{r0,r1,r2}` -- 0x58 bytes against the ROM's 0x48. There is no portable
 * way to name the caller's return slot from inside a returning method, so the
 * pointer-taking form stays, exactly as Actor's destructors stay extern "C"
 * definitions under their mangled names. The DECLARATION carries the truth; the
 * definition reproduces the bytes.
 *
 * Three shadows died even so. The old file carried a `struct Base` with thirty
 * declared virtuals whose sole purpose was to land `m74` at vtable+0x74, its own
 * `Vec`, and a `(long long)(int)` cast on the `+=`. The first two are now the
 * real Actor and the real Vector3. The cast was measured both ways and is NOT
 * doing anything -- plain `ret->y +=` is byte-identical across all 0x48 bytes --
 * so it is gone. Assuming it protected the recomputed-address shape the ROM
 * uses (`add r2,r4,#4` then a separate load-add-store) would have been wrong;
 * mwcc emits that either way.
 */
#include "Actor.h"

extern "C" void _ZN5Actor25OnAimedAtWithEggReturnVecEv(Vector3 *ret, Actor *self)
{
    ret->x = self->mPosX;
    ret->y = self->mPosY;
    ret->z = self->mPosZ;
    ret->y += self->OnAimedAtWithEgg();
}
