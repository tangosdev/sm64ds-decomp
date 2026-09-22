/* HOST COPY of src/_ZN8dActor_c23HorzAngleToCPlayerOrAngEv.cpp (lane w5-c), the
 * r0-passthrough seam Actor_ClosestPlayer_OverlayReaders.cpp's registry names
 * for exactly this TU: the matched body calls Actor::ClosestPlayer() with NO
 * argument and relies on `this` riding ARM r0. On the host, ClosestPlayer is
 * `(void *self)` cdecl, so the zero-argument call reads a stack-garbage base
 * (garbage + 0x5c, the rabbit-crash class). The one change from the matched
 * source is the ClosestPlayer declaration (one-arg) and its call site passing
 * `self` -- the value the ROM leaves in r0. Everything else, including the
 * read of the cached closest-player global data_0209b458 the call populates,
 * is the matched TU line for line.
 *
 * Reached from ov070's FlyGuy state machine (the census's class-b closure);
 * the raw source is commented out of port/slice_w5c.txt in favour of this
 * copy, per the closestplayer seat gate.
 */
extern "C" {
typedef int Fix12i;
typedef short s16;

struct Vector3 { Fix12i x, y, z; };
struct Vector3_16 { s16 x, y, z; };

struct Actor_ {
    char _pad0[0x5c];
    struct Vector3 pos;     /* @ 0x5c */
    char _pad1[0x8c - 0x68];
    struct Vector3_16 ang;  /* @ 0x8c */
};

void *_ZN8dActor_c13ClosestPlayerEv(void *self);
s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern struct Actor_ *data_0209b458; /* @ 0x0209b458, cached by ClosestPlayer */

// PORT_HOST_ABI: implicit-register-arg (ClosestPlayer's this rode r0; the host passes self).
s16 _ZN8dActor_c23HorzAngleToCPlayerOrAngEv(struct Actor_ *self)
{
    struct Actor_ *player;

    _ZN8dActor_c13ClosestPlayerEv(self);   /* <-- this, the ROM's r0 */
    player = data_0209b458;
    if (player == 0)
        return self->ang.y;

    return Vec3_HorzAngle(&self->pos, &player->pos);
}
}
