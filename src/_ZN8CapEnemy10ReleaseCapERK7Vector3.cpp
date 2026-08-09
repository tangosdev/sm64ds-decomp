//cpp
// @symbol _ZN8CapEnemy10ReleaseCapERK7Vector3
/* recovered: named members + shared header, real C++ method
 *
 * Spawns the cap this enemy is wearing as a free-standing actor at its own
 * position plus the caller's offset, and marks itself as having released it.
 *
 * Three outcomes, and the caller distinguishes them by the return value:
 *   mCapId & 7 >= 6   no cap -- returns null, nothing happens
 *   mCapId >= 6       returns THIS, not null: the marker bits are already set,
 *                     so the cap is gone but the enemy still exists. That is
 *                     what lets GetCapEatenOffIt tell "released just now" from
 *                     "was already released" with one pointer compare.
 *   otherwise         returns the new cap actor
 *
 * Which marker bit goes into mCapId depends on unk_110 -- bit 3 for the second
 * bank, bit 7 for the first.
 *
 * The spawn's second argument packs mCapId into bits 8+ of 0x1012, so one
 * actor ID covers all six caps.
 */
#include "CapEnemy.h"

struct Vector3_16 { short x, y, z; };
struct Actor;

extern "C" {
extern void func_02005ed8(unsigned char *t);
extern void Vec3_Add(struct Vector3 *out, const struct Vector3 *a, const struct Vector3 *b);
extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a1, unsigned int a2, const struct Vector3 *a3,
    const struct Vector3_16 *a4, int a5, int a6);
}

struct Actor *CapEnemy::ReleaseCap(const Vector3 & v_)
{
    unsigned char *c = (unsigned char *)this;
    const struct Vector3 *v = &v_;
    struct Actor *ret = 0;

    if ((mCapId & 7) < 6) {
        func_02005ed8(c);
        if (mCapId < 6u) {
            struct Vector3 out;
            Vec3_Add(&out, (const struct Vector3 *)&unk_05c, v);
            ret = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x10d, 0x1012 | (mCapId << 8), &out,
                (const struct Vector3_16 *)&unk_08c,
                mAreaId, -1);
            if (unk_110 != 0) {
                *(unsigned char *)(((int)c + 0x113) & 0xFFFFFFFFFFFFFFFFull) |= 8;
            } else {
                *(unsigned char *)(((int)c + 0x113) & 0xFFFFFFFFFFFFFFFFull) |= 0x80;
            }
        } else {
            ret = (struct Actor *)c;
        }
    }
    return ret;
}
