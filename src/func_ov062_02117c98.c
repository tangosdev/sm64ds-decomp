typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vec3;
typedef struct { s16 x, y, z; } V16;

#define AT(p, off) ((void*)(int)(((long long)(int)((char*)(p) + (off)))))

extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_020ada40(void* self, void* v, void* a, int f);
extern void func_ov062_02117bf4(void* self);
extern void func_020aea30(void* self, void* a, int b);
extern int _ZN6Player9IsOnShellEv(void* p);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* self, void* clsn, void* player);
extern void func_0201267c(int a, void* p);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, u32 a, int f, u32 b, u32 c, u32 d);
extern void func_ov062_02117b48(void* p);
extern void _ZN5Actor8PoofDustEv(void* self);
extern void func_ov062_021179e4(void* c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);

void func_ov062_02117c98(void* self)
{
    u8* c = (u8*)self;
    void* found;
    int r5;
    s32 flags;
    u32 id;

    id = *(u32*)(c + 0x134);
    if (id == 0)
        return;
    found = _ZN5Actor10FindWithIDEj(id);
    if (found == 0)
        return;

    flags = *(s32*)(c + 0x130);
    r5 = (int)(((long long)(int)0));

    if (flags & 0x10) {
        V16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)r5;
        v.z = (s16)r5;
        func_020ada40(self, &v, found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (*(s32*)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            return;
        }
        *(s32*)(c + 0x10c) = 5;
        func_020aea30(self, found, r5);
        return;
    }
    if (flags & 0x4000) {
        *(s32*)(c + 0x10c) = 6;
        r5 = 1;
    } else if (flags & 0x447e0) {
        if (*(s32*)(c + 0x390) == 0) {
            func_ov062_02117bf4(self);
            if (flags & 0x3c0)
                *(s16*)(c + 0x94) = *(s16*)((u8*)found + 0x8e);
        } else {
            if (flags & 0x40040)
                *(s32*)(c + 0x10c) = 2;
            else if (flags & 0x20400)
                *(s32*)(c + 0x10c) = 5;
            else if (flags & 0x380)
                *(s32*)(c + 0x10c) = 3;
            else if (flags & 0x4000)
                *(s32*)(c + 0x10c) = 6;
            else {
                *(s32*)(c + 0x10c) = 1;
                func_0201267c(0x113, (void*)(c + 0x74));
                *(s32*)(c + 0x80) = 0x1000;
                *(s32*)(c + 0x84) = 0x1000;
                *(s32*)(c + 0x88) = 0x1000;
            }
            func_020aea30(self, found, 0);
            return;
        }
    } else {
        u8* f = (u8*)found;
        struct { Vec3 sv; Vec3 hv; } L;
        int shell;
        shell = (*(u16*)(f + 0xc) == 0xbf) ? 1 : r5;
        if ((int)(((long long)shell)) == 0)
            goto tail;
        if (*(u8*)(f + 0x6f9) != 0) {
            if (*(s32*)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
                goto tail;
            }
            *(s32*)(c + 0x10c) = 6;
            func_020aea30(self, found, 0);
            return;
        }
        {
            s32* s = (s32*)AT(f, 0x5c);
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (_ZN6Player9IsOnShellEv(found)) {
            *(s32*)(c + 0x10c) = 5;
            r5 = 1;
            goto tail;
        }
        if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(self, (void*)(c + 0x110), found)) {
            if (*(s32*)(c + 0x390) == 0) {
                func_ov062_02117bf4(self);
            } else {
                *(s32*)(c + 0x10c) = 1;
                func_020aea30(self, found, 0);
                *(s32*)(c + 0x80) = 0x1000;
                *(s32*)(c + 0x84) = 0x1000;
                *(s32*)(c + 0x88) = 0x1000;
                func_0201267c(0x113, (void*)(c + 0x74));
            }
            _ZN6Player6BounceE5Fix12IiE(found, 0x28000);
            return;
        }
        if (*(u8*)(f + 0x6fb) != 0)
            goto tail;
        if ((*(s32*)(c + 0x130) & 0x400000) == 0)
            goto tail;
        if (*(s32*)(c + 0x38c) == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = *(s32*)(c + 0x5c);
            L.hv.y = *(s32*)(c + 0x60);
            L.hv.z = *(s32*)(c + 0x64);
            pw = *(s32*)(c + 0x98);
            if (pw < 0xf000)
                pw = 0xf000;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(found, &L.hv, 0, pw, 1, 0, 1);
            func_ov062_02117b48(self);
            if (*(s32*)(c + 0x390) == 2) {
                _ZN5Actor8PoofDustEv(self);
                func_ov062_021179e4(self);
                _ZN5Actor24KillAndTrackInDeathTableEv(self);
            }
        }
    }

tail:
    func_020aea30(self, found, (int)(c + 0x144));
    if (r5)
        *(u16*)(c + 0x8e) = *(s16*)(c + 0x94) + 0x8000;
}
