//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(u32 id);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(void* self, void* clsn, void* player);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, Fix12 f);
extern int func_0201267c(u32 a, void* b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12 f, u32 b, u32 c, u32 d);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(void* c, void* v, void* r4, s32 flag);
}

extern "C" int func_ov063_0211a0dc(char* c)
{
    void* r4;
    u32 id;
    Vector3 v1, v2;

    id = *(u32*)(c + 0x1a8);
    if (id == 0)
        goto ret0;

    if (*(s32*)(c + 0x1a4) & 0x207e0) {
        void* found;

        *(u32*)(int)(((long long)(int)(c + 0x19c))) |= 1;
        found = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
        if (found) {
            *(void**)(c + 0x488) = found;

            {
                int isBf = (int)(*(u16*)((char*)found + 0xc) == 0xbf);
                if (isBf) {
                    if (*(s32*)((char*)found + 8) == 3) {
                        u16* p = (u16*)(((long long)(int)(c + 0x5d4)));
                        *p |= 0x40;
                    }
                }
            }
        }
        return 1;
    }

    r4 = _ZN8dActor_c10FindWithIDEj(id);
    if (!r4)
        goto ret0;
    if (!(*(s32*)(c + 0x1a4) & 0x400000))
        goto ret0;
    if (*(u8*)((char*)r4 + 0x6fb) != 0)
        return 0;

    if (*(u8*)((char*)r4 + 0x6f9) != 0) {
        *(u32*)(int)(((long long)(int)(c + 0x19c))) |= 1;
        *(void**)(c + 0x488) = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
        return 1;
    }

    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x184, r4) != 0) {
        _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
        func_0201267c(0x149, c + 0x74);
        return -1;
    }

    {
    int isKind_d2 = (int)(*(u16*)(c + 0xc) == 0xd2);
    if (isKind_d2) {
        if (*(u8*)((char*)r4 + 0x6de) != 0) {
            _ZN6Player6BounceE5Fix12IiE(r4, 0x28000);
            func_0201267c(0x149, c + 0x74);
            return -1;
        }

        v1.x = *(s32*)(c + 0x5c);
        v1.y = *(s32*)(c + 0x60);
        v1.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v1, *(u32*)(c + 0x5ac), 0xc000, 1, 0, 1);
        func_0201267c(0x149, c + 0x74);
        return -1;
    }
    }

    if (*(s32*)(c + 0x1a4) & 0x10) {
        u8 kind = *(u8*)(c + 0x5cf);

        if (kind < 2)
            goto found_kind;
        if (kind >= 6)
            goto found_kind;
        if (kind > 9)
            goto no_kind;

found_kind:
        {
            void* found2;
            s16 v[3];

            *(u32*)(int)(((long long)(int)(c + 0x19c))) |= 1;
            found2 = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));
            if (!found2)
                goto ret0;
            *(void**)(c + 0x488) = _ZN8dActor_c10FindWithIDEj(*(u32*)(c + 0x1a8));

            v[0] = -0x2000;
            v[1] = 0;
            v[2] = 0;
            _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, found2, 0);

            {
                s16* ap = (s16*)(int)(((long long)(int)(c + 0x8e)));
                *ap = (s16)(*ap + 0x8000);
            }
            return 1;
        }
    }

no_kind:
    v2.x = *(s32*)(c + 0x5c);
    v2.y = *(s32*)(c + 0x60);
    v2.z = *(s32*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v2, *(u32*)(c + 0x5ac), 0xc000, 1, 0, 1);
    func_0201267c(0x149, c + 0x74);
    return -1;

ret0:
    return 0;
}
