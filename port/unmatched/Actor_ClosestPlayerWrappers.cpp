/* HOST COPIES of src/_ZN5Actor13DistToCPlayerEv.cpp and
 * src/_ZN5Actor14FarthestPlayerEv.cpp -- the two thin wrappers that call
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
 * extern is `_ZN5Actor13ClosestPlayerEv(void)`); byte-identical on ARM because
 * r0 already holds `this`.
 *
 * On the host the ClosestPlayer bridge (hal/reverse_bridges.cpp) is
 * `_ZN5Actor13ClosestPlayerEv(void *self)` -- cdecl, `self` off the stack. The
 * zero-argument call pushes nothing, so `self` is stack garbage and
 * ClosestPlayer measures distance from garbage+0x5c, faulting or returning a
 * meaningless cache. THE FIX passes `this` explicitly, exactly the value the
 * ROM leaves in r0.
 *
 * src/_ZN5Actor13DistToCPlayerEv.c and src/_ZN5Actor14FarthestPlayerEv.c are
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
 * (src/_ZN6Rabbit8BehaviorEv.c, both call sites).
 *
 * It is intermittent because ClosestPlayer is cached: the body only runs the
 * loop when data_0209b458 is null, so a bad `this` is harmless on every frame
 * the cache is warm. That is why one reporter hit it and a walk never did.
 *
 * The five sources are dropped from gate 16, 18, 33 and 177 in favour of the
 * copies below; the byte-locked sources are unchanged.
 */

extern "C" {

void *_ZN5Actor13ClosestPlayerEv(void *self);   /* the real one-arg (this) shape */

extern int   data_0208e380;   /* closest-player distance, set by ClosestPlayer */
extern void *data_0209b450;   /* farthest-player pointer,  set by ClosestPlayer */

/* Actor::DistToCPlayer() -> s32 */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN5Actor13DistToCPlayerEv(void *self)
{
    _ZN5Actor13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    return data_0208e380;
}

/* Actor::FarthestPlayer() -> Player* */
// PORT_HOST_ABI: implicit-register-arg (FarthestPlayer: same shape, ClosestPlayer's this rode r0; the host passes it).
void *_ZN5Actor14FarthestPlayerEv(void *self)
{
    _ZN5Actor13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
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

/* Actor::HorzAngleToCPlayer() -> s16.  Body is the matched source verbatim
   except for the argument the ROM left in r0. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
s16 _ZN5Actor18HorzAngleToCPlayerEv(struct PortActor *self)
{
    _ZN5Actor13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    return Vec3_HorzAngle(&self->pos, &data_0209b458->pos);
}

/* Actor::HorzAngleToFPlayer() -> s16.  Reads the farthest-player global that
   the same call refills. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
s16 _ZN5Actor18HorzAngleToFPlayerEv(struct PortActor *self)
{
    _ZN5Actor13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    return Vec3_HorzAngle(&self->pos,
                          &((struct PortActor *)data_0209b450)->pos);
}

/* Actor::IsPlayerInRange(s32 maxDist) -> bool.  maxDist arrives as a whole-unit
   integer and is shifted into 20.12 at the comparison, per Actor.h. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN5Actor15IsPlayerInRangeEi(struct PortActor *self, int maxDist)
{
    struct PortActor *closest =
        (struct PortActor *)_ZN5Actor13ClosestPlayerEv(self);  /* <-- this */
    return Vec3_Dist(&self->pos, &closest->pos) < (maxDist << 12);
}

/* Actor::IsPlayerInRange(const Vector3 &pos, s32 maxDist) -> bool.  The body
   never touches a field of `this`; it is a member because the ROM puts pos in
   r1 and maxDist in r2, leaving r0 written-but-unread -- and r0 is exactly what
   ClosestPlayer goes on to read. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN5Actor15IsPlayerInRangeERK7Vector3i(struct PortActor *self,
                                           const struct PortVec3 *pos,
                                           int maxDist)
{
    struct PortActor *closest =
        (struct PortActor *)_ZN5Actor13ClosestPlayerEv(self);  /* <-- this */
    return Vec3_Dist(pos, &closest->pos) < (maxDist << 12);
}

/* Actor::IsPlayerInRange(Fix12i, Fix12i, Fix12i, s32) -> bool. */
// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0 from the enclosing member; the host passes it).
int _ZN5Actor15IsPlayerInRangeE5Fix12IiES1_S1_i(struct PortActor *self,
                                                Fix12i posX, Fix12i posY,
                                                Fix12i posZ, int maxDist)
{
    struct PortVec3 pos;
    struct PortActor *closest;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    closest = (struct PortActor *)_ZN5Actor13ClosestPlayerEv(self); /* <-- this */
    return Vec3_Dist(&pos, &closest->pos) < (maxDist << 12);
}

}  /* extern "C" */
