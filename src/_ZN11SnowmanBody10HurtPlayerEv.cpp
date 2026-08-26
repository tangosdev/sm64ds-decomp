//cpp
#include "SnowmanBody.h"
#include "common.h"

extern "C" dActor_c *_ZN8dActor_c10FindWithIDEj(u32 id);
extern "C" void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    dActor_c *actor, const Vector3 *pos, u32 a, int fix, u32 b, u32 c, u32 d);

int SnowmanBody::HurtPlayer()
{
    char *self = (char *)this;
    dActor_c *actor;
    u32 id;
    int t;
    Vector3 pos;

    id = *(u32 *)(self + 0x170);
    if (id == 0) return 0;
    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;
    t = (int)(*(u16 *)((char *)actor + 0xc) == 0xbf);
    if (t != 0) goto body;
fail:
    return 0;
body:
    if (*(u32 *)(self + 0x98) != 0) {
        pos.x = *(int *)(self + 0x5c);
        pos.y = *(int *)(self + 0x60);
        pos.z = *(int *)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
            actor, &pos, 2, 0xc000, 1, 0, 1);
    }
    return 1;
}
