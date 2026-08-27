/* DotVec3 at 0x0205380c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef int Fix12i;
typedef struct Vector3 { Fix12i x, y, z; } Vector3;

Fix12i DotVec3(const Vector3 *a, const Vector3 *b)
{
    long long sum = (long long)a->x * b->x
                  + (long long)a->y * b->y
                  + (long long)a->z * b->z;
    return (Fix12i)((sum + 0x800) >> 12);
}
