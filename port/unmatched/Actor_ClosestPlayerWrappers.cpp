/* HOST COPIES of src/_ZN8dActor_c13DistToCPlayerEv.cpp and
 * src/_ZN8dActor_c14FarthestPlayerEv.cpp -- the two thin wrappers that call
 * Actor::ClosestPlayer() for its side effect (it caches the closest distance
 * and the farthest-player pointer in file-scope globals) and then return one
 * of those globals.
 *
 * THE CALLING-CONVENTION SEAM:
 *
 * Both callers are Actor member functions; each receives its own `this` in r0.
 * Actor::ClosestPlayer() is ALSO a member -- it reads `this` (r0) to measure
 * distance from `this` to every player: Vec3_Dist((char*)this + 0x5c, ...).
 * On the DS the two wrappers call it without touching r0, so ClosestPlayer's
 * `this` arrives for free as the wrapper's own `this`:
 *
 *     DistToCPlayer (0x020109e4):   stmdb sp!,{lr}; sub sp,#4
 *                                   bl   ClosestPlayer   ; r0 still = this
 *                                   ldr  r0,[pc]; ldr r0,[r0]   ; = data_0208e380
 *     FarthestPlayer (0x02010958):  same shape, returns data_0209b450
 *
 * The matched C spells the call as ClosestPlayer() with no argument (the src
 * extern is `_ZN8dActor_c13ClosestPlayerEv(void)`); byte-identical on ARM because
 * r0 already holds `this`.
 *
 * On the host the ClosestPlayer bridge (hal/reverse_bridges.cpp) is
 * `_ZN8dActor_c13ClosestPlayerEv(void *self)` -- cdecl, `self` off the stack. The
 * zero-argument call pushes nothing, so `self` is stack garbage and
 * ClosestPlayer measures distance from garbage+0x5c, faulting or returning a
 * meaningless cache. THE FIX passes `this` explicitly, exactly the value the
 * ROM leaves in r0.
 *
 * src/_ZN8dActor_c13DistToCPlayerEv.cpp and src/_ZN8dActor_c14FarthestPlayerEv.cpp are
 * dropped from their slice files (gate 16, gate 89) in favour of this file; the
 * byte-locked sources are unchanged.
 *
 * THE SAME SEAM IN FIVE MORE READERS (the RABBIT's 0x5c fault).
 *
 * Actor.h names nine readers of the closest-player cache and says every one of
 * them "opens by calling ClosestPlayer()". Only the two above were given the
 * argument. The other seven kept the zero-argument call, and five of those five
 * are in the build: IsPlayerInRange in its three overloads, HorzAngleToCPlayer
 * and HorzAngleToFPlayer. (HorzAngleToCPlayerOrAng is in no slice, and
 * ClosestNonVanishPlayer makes no such call.)
 *
 * The live crash that proves it is in the corpus, id 9e164e92, reporter build
 * 0.1.1 / 6a648a490, walker class RABBIT, actor id 187:
 *
 *     access 00000000 at 0000005c
 *     ecx 0000005c   eax 0581ef1c   esi 0581eec0 (the player)
 *
 * ClosestPlayer's loop is Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c). With `c`
 * the stack garbage a zero-argument cdecl call leaves behind, and that garbage
 * happening to be 0, the first argument is 0x5c and the second is the player at
 * 0581eec0 plus 0x5c = 0581ef1c. Both registers in the dump are exactly those
 * two values, so the faulting read is ClosestPlayer measuring from a null
 * `this` -- not the rabbit's own state machine, which passes `c` correctly
 * (src/actors/daMip_c.cpp, both call sites).
 *
 * It is intermittent because ClosestPlayer is cached: the body only runs the
 * loop when data_0209b458 is null, so a bad `this` is harmless on every frame
 * the cache is warm. That is why one reporter hit it and a walk never did.
 *
 * The five sources are dropped from gate 16, 18, 33 and 177 in favour of the
 * copies below; the byte-locked sources are unchanged.
 *
 * SEAT15B (run link100 wave 15) retired three of those five: HorzAngleToCPlayer,
 * HorzAngleToFPlayer and IsPlayerInRange(s32) are real C++ members in src/ now,
 * so their matched TUs hold the seats. What is left here is DistToCPlayer,
 * FarthestPlayer and the two IsPlayerInRange overloads with no seat of their own.
 */

extern "C" {

void *_ZN8dActor_c13ClosestPlayerEv(void *self);   /* the real one-arg (this) shape */

extern int   data_0208e380;   /* closest-player distance, set by ClosestPlayer */
extern void *data_0209b450;   /* farthest-player pointer,  set by ClosestPlayer */

/* Actor::DistToCPlayer() -> s32 */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN8dActor_c13DistToCPlayerEv(void *self)
{
    _ZN8dActor_c13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    return data_0208e380;
}

/* Actor::FarthestPlayer() -> Player* */
// PORT_HOST_ABI: implicit-register-arg (FarthestPlayer: same shape, ClosestPlayer's this rode r0; the host passes it).
void *_ZN8dActor_c14FarthestPlayerEv(void *self)
{
    _ZN8dActor_c13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    return data_0209b450;
}

/* ---- the five readers that kept the zero-argument call -------------------- */

typedef int   Fix12i;    /* 20.12 fixed point, as the matched sources spell it */
typedef short s16;

struct PortVec3 { Fix12i x, y, z; };
struct PortActor { char _pad[0x5c]; struct PortVec3 pos; };  /* pos @ 0x5c */

extern Fix12i Vec3_Dist(const struct PortVec3 *a, const struct PortVec3 *b);
extern s16    Vec3_HorzAngle(const struct PortVec3 *v0, const struct PortVec3 *v1);
extern struct PortActor *data_0209b458;   /* closest-player pointer */

/* RETIRED at run link100 wave 15, lane SEAT15B (LINK15 BATCH 1):
   Actor::HorzAngleToCPlayer, Actor::HorzAngleToFPlayer and
   Actor::IsPlayerInRange(s32). Their matched sources are real C++ members now
   (src/_ZN8dActor_c18HorzAngleToCPlayerEv.cpp, ...FPlayerEv.cpp,
   src/_ZN8dActor_c15IsPlayerInRangeEi.cpp), so ClosestPlayer() is called on
   `this` and there is nothing left to work around. They are seated on
   port/slice_l15cp.txt, and the flat ROM names the extern "C" callers spell are
   carried by reverse faces rather than by a host body: the two angle readers by
   derived rows in port/faces_sync.txt (HorzAngleToCPlayer's row was a FORWARD row
   until this lane flipped it), IsPlayerInRange(s32) by the hand-written bridge
   below, which facegen refuses to derive for a reason worth reading. The two IsPlayerInRange overloads below KEEP their host copies:
   re-read at 8ddff3187, src/_ZN8dActor_c15IsPlayerInRangeERK7Vector3i.cpp is
   seated already as a member and needs no flat body here, and the Fix12i
   overload has no matched TU of its own. */

/* THE ONE REVERSE BRIDGE THIS FILE STILL OWES.

   The other two seats of this sub-batch get their flat ROM name from a derived
   reverse face (port/faces_sync.txt). This one cannot: facegen refuses the row
   on its own rule 2, because three ROM addresses join dActor_c::IsPlayerInRange
   (0x0201045c the Vector3 overload, 0x02010498 the Fix12i one, 0x020104dc this
   one) and the counted parameter subset does not tell them apart. Refusing to
   guess is the right answer for a derivation. A hand-written bridge has no such
   ambiguity: it names the overload at the call site, so the one int argument
   picks dActor_c::IsPlayerInRange(s32) and nothing else.

   The shadow declaration below is a DECLARATION and never a body: it exists only
   to make MSVC emit a call to ?IsPlayerInRange@dActor_c@@QAE_NH@Z, which is the
   decorated name src/_ZN8dActor_c15IsPlayerInRangeEi.cpp defines. The return is
   widened bool -> int on purpose -- facegen's RETURN WIDTH RULE, port/tools/
   facegen.py line 147: the member writes AL alone, the flat name is extern "C"
   so the linker cannot see a caller that spells it int, and the cartridge's own
   body defines the whole register. */
struct dActor_c { bool IsPlayerInRange(int maxDist); };

extern "C" int _ZN8dActor_c15IsPlayerInRangeEi(void *self, int maxDist)
{ return (int)((struct dActor_c *)self)->dActor_c::IsPlayerInRange(maxDist); }

/* Actor::IsPlayerInRange(const Vector3 &pos, s32 maxDist) -> bool.  The body
   never touches a field of `this`; it is a member because the ROM puts pos in
   r1 and maxDist in r2, leaving r0 written-but-unread -- and r0 is exactly what
   ClosestPlayer goes on to read. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN8dActor_c15IsPlayerInRangeERK7Vector3i(struct PortActor *self,
                                           const struct PortVec3 *pos,
                                           int maxDist)
{
    struct PortActor *closest =
        (struct PortActor *)_ZN8dActor_c13ClosestPlayerEv(self);  /* <-- this */
    return Vec3_Dist(pos, &closest->pos) < (maxDist << 12);
}

/* Actor::IsPlayerInRange(Fix12i, Fix12i, Fix12i, s32) -> bool. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN8dActor_c15IsPlayerInRangeE5Fix12IiES1_S1_i(struct PortActor *self,
                                                Fix12i posX, Fix12i posY,
                                                Fix12i posZ, int maxDist)
{
    struct PortVec3 pos;
    struct PortActor *closest;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    closest = (struct PortActor *)_ZN8dActor_c13ClosestPlayerEv(self); /* <-- this */
    return Vec3_Dist(&pos, &closest->pos) < (maxDist << 12);
}

}  /* extern "C" */
