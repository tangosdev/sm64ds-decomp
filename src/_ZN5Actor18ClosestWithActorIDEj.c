/* Actor::ClosestWithActorID(u32 actorID)
 * Walks every actor with the given actorID and returns the one nearest to
 * this actor (excluding `this`). Returns null if none found. */

typedef int Fix12i; /* 20.12 fixed-point */

struct Vector3 { Fix12i x, y, z; }; /* 0xc */

struct Actor {
    char _pad[0x5c];
    struct Vector3 pos; /* 0x5c */
};

/* Resolved callees (symbols/verified.tsv):
 *   0x02010ef0 = Actor::FindWithActorID(u32 actorID, Actor* searchStart)
 *   0x0203cfdc = Vec3_Dist(const Vector3&, const Vector3&) */
extern struct Actor *_ZN5Actor15FindWithActorIDEjPS_(unsigned int actorID, struct Actor *searchStart);
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);

struct Actor *_ZN5Actor18ClosestWithActorIDEj(struct Actor *self, unsigned int actorID) {
    struct Actor *actor = 0;
    struct Actor *closest = 0;
    Fix12i closestDist = 0x7fffffff;

    for (;;) {
        actor = _ZN5Actor15FindWithActorIDEjPS_(actorID, actor);
        if (!actor)
            break;
        if (actor != self) {
            Fix12i dist = Vec3_Dist(&self->pos, &actor->pos);
            if (dist < closestDist) {
                closestDist = dist;
                closest = actor;
            }
        }
    }
    return closest;
}
