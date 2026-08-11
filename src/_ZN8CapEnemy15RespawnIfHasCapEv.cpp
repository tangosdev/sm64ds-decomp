//cpp
// @symbol _ZN8CapEnemy15RespawnIfHasCapEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "CapEnemy.h"
struct Actor;

extern "C" {
extern struct Actor *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int a1, unsigned int a2, const struct Vector3 *a3,
    const struct Vector3_16 *a4, int a5, int a6);
}

struct Actor * CapEnemy::RespawnIfHasCap()
{
    struct Actor *r;
    func_02005ed8(((unsigned char *)this));
    if ((((unsigned char *)this)[0x113] & 0xf) >= 6) return 0;
    r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
        actorID, param1,
        (const struct Vector3 *)((unsigned char *)&mPosX),
        (const struct Vector3_16 *)((unsigned char *)&mAngleX),
        mAreaId, -1);
    if (!r) return r;
    *(unsigned char *)((char *)r + 0x111) = 1;
    *(int *)((char *)r + 0xf4) = *(int *)((char *)r + 0xb0);
    *(unsigned char *)((char *)r + 0x108) = 0;
    {
        int *p = (int *)((unsigned long long)((char *)r + 0xb0));
        *p &= ~1;
        *p &= ~0x10000000;
    }
    return r;
}
