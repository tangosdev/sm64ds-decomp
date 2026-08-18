/* Actor::IsPlayerInRange(Fix12i posX, Fix12i posY, Fix12i posZ, s32 maxDist)
 * Builds a Vector3 from the three coordinates and tests whether the closest
 * player is within maxDist of it. */

typedef int Fix12i; /* 20.12 fixed-point */

struct Vector3 { Fix12i x, y, z; }; /* 0xc */

struct Actor {
    char _pad[0x5c];
    struct Vector3 pos; /* 0x5c */
};

/* Resolved callees (symbols/verified.tsv):
 *   0x02010ad8 = Actor::ClosestPlayer()
 *   0x0203cfdc = Vec3_Dist(const Vector3&, const Vector3&) */
extern struct Actor *_ZN5Actor13ClosestPlayerEv(void);
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);

int _ZN5Actor15IsPlayerInRangeE5Fix12IiES1_S1_i(struct Actor *self, Fix12i posX, Fix12i posY, Fix12i posZ, int maxDist) {
    struct Vector3 pos;
    pos.x = posX;
    pos.y = posY;
    pos.z = posZ;
    struct Actor *closest = _ZN5Actor13ClosestPlayerEv();
    return Vec3_Dist(&pos, &closest->pos) < (maxDist << 12);
}
