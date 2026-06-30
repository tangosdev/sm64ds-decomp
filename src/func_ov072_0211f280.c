/* func_ov072_0211f280 at 0x0211f280
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov072).
 */
typedef unsigned short u16;
typedef unsigned int u32;

struct Actor;
struct Vector3 { int x, y, z; };

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(struct Actor* actor, const struct Vector3* pos, u32 a, int fix, u32 b, u32 c, u32 d);

int func_ov072_0211f280(char* self)
{
    struct Actor* actor;
    u32 id;
    int t;
    struct Vector3 pos;

    id = *(u32*)(self + 0x170);
    if (id == 0) return 0;

    actor = _ZN5Actor10FindWithIDEj(id);
    if (actor == 0) goto fail;

    t = (int)(*(u16*)((char*)actor + 0xc) == 0xbf);
    if (t != 0) goto body;

fail:
    return 0;

body:
    if (*(u32*)(self + 0x98) != 0) {
        pos.x = *(int*)(self + 0x5c);
        pos.y = *(int*)(self + 0x60);
        pos.z = *(int*)(self + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos, 2, 0xc000, 1, 0, 1);
    }
    return 1;
}