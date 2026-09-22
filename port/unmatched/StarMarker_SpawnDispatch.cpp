/* HOST COPY of src/func_ov002_020b10a0.c -- kills an actor and, if a matching
 * StarMarker exists, asks that StarMarker to spawn its red-coin star.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * StarMarker::SpawnRedCoinStarIfNecessary reads `this` (its host body does
 * `mov r5, r0` then `ldrb [r5, #0x1db]`). On the DS the caller leaves the found
 * StarMarker live in r0 at the call:
 *
 *     020b10bc: mov  r0, r4               ; r0 = c (the dying actor)
 *     020b10c0: bl   func_ov002_020b1328  ; returns the found StarMarker in r0
 *     020b10c4: cmp  r0, #0
 *     020b10c8: beq  0x020b10d0
 *     020b10cc: bl   SpawnRedCoinStarIfNecessary   ; r0 still = the StarMarker
 *
 * func_ov002_020b1328 returns the Actor with id 0xb4 whose fields match `c`
 * (its src returns `r1`, the FindWithActorID result); that Actor IS the
 * StarMarker, and it stays in r0 straight into the Spawn call. The matched C
 * spells the call with no argument (`SpawnRedCoinStarIfNecessary(void)`);
 * byte-identical on ARM because the StarMarker is already in r0.
 *
 * On the host the SpawnRedCoinStarIfNecessary bridge takes `this` off the stack;
 * the zero-argument call pushes nothing, so it reads garbage and dereferences it
 * at +0x1db / +0x5c. THE FIX captures func_ov002_020b1328's return -- exactly
 * the StarMarker the ROM leaves in r0 -- and passes it explicitly. Behaviour is
 * preserved: it is still only called when that return is non-null, matching the
 * `if (func_ov002_020b1328(c))` guard.
 *
 * src/func_ov002_020b10a0.c is dropped from slice_gate33.txt in favour of this
 * file; the byte-locked source is unchanged.
 */

extern "C" {
extern int _ZN8dActor_c18GetBitInDeathTableEv(void*);
extern void* func_ov002_020b1328(void*);
extern void _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(void* self); /* real one-arg (this) */
extern int _ZN8dActor_c24KillAndTrackInDeathTableEv(void*);
// PORT_HOST_ABI: implicit-register-arg (SpawnRedCoinStarIfNecessary receiver = the found marker, rode r0 from func_ov002_020b1328).
int func_ov002_020b10a0(char* c){
  if(_ZN8dActor_c18GetBitInDeathTableEv(c)==0) return 0;
  {
    void* sm = func_ov002_020b1328(c);            /* the found StarMarker, ROM's r0 */
    if(sm) _ZN10StarMarker27SpawnRedCoinStarIfNecessaryEv(sm);
  }
  _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
  return 1;
}
}
