//cpp
#include "types.h"
// @symbol _ZN5Ukiki8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_SaveData.h"
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Ukiki.h"
struct Vector3;
struct Vector3_16;
struct dActor_c;

extern "C" {
extern int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(dActor_c *thiz, int d);
extern dActor_c *_ZN8dActor_c13ClosestPlayerEv(dActor_c *thiz);
extern void *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(u32 a, u32 b, const Vector3 *c, const Vector3_16 *d, int e, int f);
extern void func_ov030_021141a8(dActor_c *c);
extern void func_ov030_02114134(dActor_c *c);
extern void func_ov030_02112094(dActor_c *c);
}

struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
};

int Ukiki::Behavior()
{
    char *c = (char *)((dActor_c *)this);
    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((dActor_c *)this), 0x5dc000) != 0 &&
        *(int *)(c + 0x3b4) != 8) {
        int b = (*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0 && *(unsigned char *)(c + 0x3c8) == 0 && _ZN8SaveData16HasPlayerLostCapEv() != 0) {
            dActor_c *pl = _ZN8dActor_c13ClosestPlayerEv(((dActor_c *)this));
            unsigned cp = *(unsigned *)((char *)pl + 8);
            if (cp < 3) {
                void *spawned;
                *(int *)(c + 0x3b0) = cp;
                spawned = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x10d,
                    ((unsigned)*(int *)(c + 0x3b0) << 8) | 2,
                    (const Vector3 *)(c + 0x5c),
                    (const Vector3_16 *)0,
                    (int)*(signed char *)(c + 0xcc),
                    -1);
                *(int *)(c + 0x3ac) = *(int *)((char *)spawned + 4);
                *(unsigned char *)(c + 0x3c8) = 1;
                func_ov030_021141a8(((dActor_c *)this));
            }
        }
        func_ov030_02111734(c);
    } else {
        func_ov030_02114134(((dActor_c *)this));
        ((VObj *)(c + 0xd4))->v3();
        func_ov030_02112094(((dActor_c *)this));
    }
    return 1;
}
