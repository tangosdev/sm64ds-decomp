typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

struct Actor;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov030_021141a8(char* self, int a);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(char* p, char* self, u32 msg, const struct Vector3* pos, u32 a, u32 b);
extern void func_0201267c(u32 id, char* p);
extern int _ZN6Player12GetTalkStateEv(char* p);
extern void _ZN6Player9DropActorEv(char* p);
extern struct Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 param, const struct Vector3* pos, const struct Vector3_16* rot, int a, int b);
extern void _ZN9ActorBase18MarkForDestructionEv(char* self);

int func_ov030_02113094(char* self)
{
    {
        int b = (int)((*(u32*)(self + 0xb0) & 0x40000) != 0);
        if (b != 0) {
            int p = (int)((((int)*(char**)(self + 0x3a8)) + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            *(int*)(self + 0x5c) = *(int*)p;
            *(int*)(self + 0x60) = *(int*)(p + 4);
            *(int*)(self + 0x64) = *(int*)(p + 8);
        }
    }

    switch (*(u8*)(self + 0x3c7)) {
    case 0: {
        int b2 = (int)((*(u32*)(self + 0xb0) & 0x40000) != 0);
        if (b2 != 0) {
            if (*(u8*)(self + 0x3c8) != 0) {
                struct Actor* a = _ZN5Actor10FindWithIDEj(*(u32*)(self + 0x3ac));
                *(char**)((char*)a + 0xd0) = *(char**)(self + 0x3a8);
                *(u32*)(((int)a + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) |= 0x40000;
            }
            (*(u8*)(((int)self + 0x3c7) & 0xFFFFFFFFFFFFFFFFLL))++;
        } else {
            int b3 = (int)((*(u32*)(self + 0xb0) & 0x20000) != 0);
            if (b3 != 0) break;
            if (b2 != 0) break;
            *(int*)(self + 0xd0) = 0;
            func_ov030_021141a8(self, *(int*)(self + 0x3b8));
        }
        break;
    }
    case 1: {
        int msg = (*(u8*)(self + 0x3c8) != 0) ? 0xc2 : 0xc3;
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(char**)(self + 0x3a8), self, (s16)msg, 0, 0, 0) != 0) {
            func_0201267c(0xd1, self + 0x74);
            (*(u8*)(((int)self + 0x3c7) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        {
            int b4 = (int)((*(u32*)(self + 0xb0) & 0x80000) != 0);
            if (b4 != 0) {
                func_ov030_021141a8(self, 7);
            }
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(char**)(self + 0x3a8)) == -1) {
            _ZN6Player9DropActorEv(*(char**)(self + 0x3a8));
            (*(u8*)(((int)self + 0x3c7) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 3: {
        int b5 = (int)((*(u32*)(self + 0xb0) & 0x80000) != 0);
        if (b5 != 0) {
            if (*(u8*)(self + 0x3c8) != 0) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x10d, (*(u32*)(self + 0x3b0) << 8) | 5, (struct Vector3*)(*(char**)(self + 0x3a8) + 0x5c), 0, *(s8*)(self + 0xcc), -1);
                _ZN9ActorBase18MarkForDestructionEv((char*)_ZN5Actor10FindWithIDEj(*(u32*)(self + 0x3ac)));
                {
                    u32 z = 0;
                    *(u32*)(self + 0x3ac) = z;
                    *(u8*)(self + 0x3c8) = (u8)z;
                    *(int*)(self + 0x3b8) = 0xa;
                }
            }
            func_ov030_021141a8(self, 7);
        }
        break;
    }
    }

    return 1;
}
