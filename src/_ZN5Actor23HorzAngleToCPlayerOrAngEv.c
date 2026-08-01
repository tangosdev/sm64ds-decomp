/* Actor::HorzAngleToCPlayerOrAng()
 * Recomputes the closest player (cached into a file-scope global). If there is
 * no closest player (null), returns this actor's own facing angle (ang.y);
 * otherwise returns the horizontal angle from this actor's position to the
 * closest player's position.
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
struct Vector3_16 { s16 x, y, z; };

struct Actor {
    char _pad0[0x5c];
    struct Vector3 pos;     /* @ 0x5c */
    char _pad1[0x8c - 0x68];
    struct Vector3_16 ang;  /* @ 0x8c */
};

extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern struct Actor *data_0209b458; /* @ 0x0209b458 */

s16 _ZN5Actor23HorzAngleToCPlayerOrAngEv(struct Actor *self)
{
    struct Actor *player;

    _ZN5Actor13ClosestPlayerEv();
    player = data_0209b458;
    if (player == 0)
        return self->ang.y;

    return Vec3_HorzAngle(&self->pos, &player->pos);
}
