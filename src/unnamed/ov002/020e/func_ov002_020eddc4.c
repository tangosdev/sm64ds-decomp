#include "types.h"
// @symbol func_ov002_020eddc4
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
/* func_ov002_020eddc4 at 0x020eddc4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */
struct dActor_c;


extern struct dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern int func_ov002_020ed63c(char* c, int i);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(struct dActor_c* self, const struct Vector3* pos, u32 a, int fix, u32 b, u32 c, u32 d);
extern void func_ov002_020edca4(char* c);

int func_ov002_020eddc4(char* self)
{
    struct dActor_c* actor;
    struct dActor_c* other;
    u32 id;
    u32 flags;
    u16 type;

    id = *(u32*)(self + 0x134);
    if (id == 0) goto fail;

    actor = _ZN8dActor_c10FindWithIDEj(id);
    if (actor == 0) goto fail;

    if (actor == *(struct dActor_c**)(self + 0x38c)) goto fail;

    {
        int t = (int)(*(u16*)((char*)actor + 0xc) == 0xbf);
        if (t != 0) {
            flags = *(u32*)(self + 0x130);
            if (flags & 0x8000) {
                func_ov002_020ed63c(self, 3);
                return 1;
            }
            if (!(flags & 0x26fe0)) {
                struct Vector3 pos;
                pos.x = *(int*)(self + 0x5c);
                pos.y = *(int*)(self + 0x60);
                pos.z = *(int*)(self + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos, 1, 0xc000, 1, 0, 1);
            }
        }
    }

    type = *(u16*)((char*)actor + 0xc);
    switch (type) {
    case 0xbd:
    case 0xbe:
    case 0xc6:
    case 0xca:
    case 0xdb:
    case 0x151:
        func_ov002_020edca4(self);
        return 1;
    }

    other = 0;
    id = *(u32*)(self + 0x410);
    if (id != 0) {
        other = _ZN8dActor_c10FindWithIDEj(id);
    }
    if (other != actor) goto fail;

    func_ov002_020ec728(self);
    func_ov002_020ed63c(self, 1);
    return 1;

fail:
    return 0;
}
