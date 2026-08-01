/* Actor::HorzAngleToCPlayer()
 * Recomputes the closest player (stored into a file-scope global as a side
 * effect), then returns the horizontal angle from this actor's position to
 * the cached closest player's position.
 *
 * Callees (symbols/verified.tsv):
 *   0x02010ad8 = Actor::ClosestPlayer() -> _ZN5Actor13ClosestPlayerEv
 *   0x0203b7ac = Vec3_HorzAngle(const Vector3&, const Vector3&)
 * Global:
 *   0x0209b458 = closest-player pointer, populated by ClosestPlayer()
 */

typedef int Fix12i; /* 20.12 fixed-point */
typedef short s16;

struct Vector3 { Fix12i x, y, z; };

struct Actor {
    char _pad[0x5c];
    struct Vector3 pos; /* @ 0x5c */
};

extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern struct Actor *data_0209b458; /* @ 0x0209b458 */

s16 _ZN5Actor18HorzAngleToCPlayerEv(struct Actor *self)
{
    _ZN5Actor13ClosestPlayerEv();
    return Vec3_HorzAngle(&self->pos, &data_0209b458->pos);
}
