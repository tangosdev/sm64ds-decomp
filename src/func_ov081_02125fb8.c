/* func_ov081_02125fb8 at 0x02125fb8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov081).
 */
enum Bool { FALSE, TRUE };

typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Vector3 { int x, y, z; } Vector3;

extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, int, int, int);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* self, const Vector3* pos, u32 a, int b, u32 c, u32 d, u32 e);
extern void func_02012694(int a, void* b);

void func_ov081_02125fb8(void* thisp) {
    char* self = (char*)thisp;
    void* other;
    int flags;
    u32 id = *(u32*)(self + 0x134);
    if (id == 0) return;
    other = _ZN5Actor10FindWithIDEj(id);
    if (other == 0) return;
    flags = *(int*)(self + 0x130);
    if ((flags & 0x10) || (flags & 0x40000)) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x11c, *(int*)(self+0x5c), *(int*)(self+0x60), *(int*)(self+0x64));
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }
    {
        enum Bool eq = (enum Bool)(*(u16*)((char*)other + 0xc) == 0xbf);
        if (eq != FALSE) {
            if (*(u8*)((char*)other + 0x6fb) != 0) return;
            if (*(u8*)((char*)other + 0x6f9) == 1) {
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x11c, *(int*)(self+0x5c), *(int*)(self+0x60), *(int*)(self+0x64));
                _ZN9ActorBase18MarkForDestructionEv(self);
                return;
            }
            {
                Vector3 pos;
                pos.x = *(int*)(self+0x5c);
                pos.y = *(int*)(self+0x60);
                pos.z = *(int*)(self+0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 1, 0xc000, 1, 0, 1);
                func_02012694(0x3c, self + 0x74);
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x11c, *(int*)(self+0x5c), *(int*)(self+0x60), *(int*)(self+0x64));
                _ZN9ActorBase18MarkForDestructionEv(self);
                return;
            }
        }
    }
}