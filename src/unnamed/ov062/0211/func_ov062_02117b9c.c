typedef int Fix12i;
typedef struct Vector3 { int x, y, z; } Vector3;
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *start);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);

void *func_ov062_02117b9c(void *c) {
    void *found = 0;
    void *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = _ZN8dActor_c15FindWithActorIDEjPS_(0x11d, found);
        if (!found) break;
        Fix12i d = Vec3_Dist((Vector3*)((char*)c + 0x5c), (Vector3*)((char*)found + 0x5c));
        if (d < bestDist) {
            bestDist = d;
            best = found;
        }
    }
    return best;
}
