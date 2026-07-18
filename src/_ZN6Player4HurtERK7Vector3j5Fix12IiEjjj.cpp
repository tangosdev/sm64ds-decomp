//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern "C" {
extern int func_ov002_020d82f0(void* c);
extern int _ZN6Player8HasNoCapEv(char* c);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, void* v);
extern void func_ov002_020d91b8(char* self, int a);
extern void func_ov002_020d5cec(char* c);
extern s16 Vec3_HorzAngle(const void* v0, const void* v1);
extern int AngleDiff(int a, int b);
extern void func_02014fa4(char* c);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, int a, int b, int cc);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int func_ov002_020d91e0(char* thiz, int damage, int doPre);

extern u16 data_ov002_020ff128[];
extern int data_ov002_02110094;
extern int data_ov002_0211010c;
extern int data_ov002_021100ac;
}

extern "C" int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* c, void* a, u32 b, int dmg, u8 arg4, u8 arg5, u8 arg6)
{
    s16 ang;
    void* p360;
    int hasCap;

    if (func_ov002_020d82f0(c) == 0) {
        return 0;
    }

    if (arg5 != 0) {
        if ((int)b < 2) {
            b += 2;
        }
    }

    hasCap = _ZN6Player8HasNoCapEv(c);
    if (hasCap != 0) goto L88;
    if (*(u8*)(c + 0x6d9) == *(int*)(c + 8)) goto L88;
    if (*(u8*)(c + 0x6f9) != 0) goto L88;
    if (b == 0) goto L88;

    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x22, c + 0x74);
    goto L9c;

L88:
    {
        int t = 0;
        if ((int)b > 1) t = 1;
        func_ov002_020d91b8(c, t);
    }

L9c:
    if (*(u8*)(c + 0x6f9) != 0) {
        b = 0;
    }
    p360 = *(void**)(c + 0x360);
    if (p360 != 0) {
        int match = (*(u16*)((char*)p360 + 0xc) == 0xbf);
        if (match) {
            func_ov002_020d5cec((char*)p360);
            *(u16*)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
        }
        *(u8*)(c + 0x714) = 0;
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)(c + 0x360) + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
            *p |= 0x80000;
        }
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)(c + 0x360) + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0x40000;
        }
        {
            int* p = (int*)(((long long)(int)((char*)*(void**)(c + 0x360) + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
            *p &= ~0x20000;
        }
        *(void**)(c + 0x360) = 0;
    }

    *(u8*)(c + 0x6e3) = 0;

    if (b != 0) {
        int neq = 0;
        if (*(u8*)(c + 0x6d9) != *(int*)(c + 8)) neq = 1;
        if (neq) *(u8*)(c + 0x6e3) = 2;
    }
    if ((int)b > 1) {
        *(u8*)(c + 0x6e3) = 2;
    }
    if (*(u8*)(c + 0x6de) != 0) {
        *(u8*)(c + 0x6e3) = 4;
    }

    ang = Vec3_HorzAngle(c + 0x5c, a);
    if (AngleDiff(*(s16*)(c + 0x8e), ang) > 0x4000) {
        *(u8*)(((long long)(int)(c + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
    }
    *(s16*)(c + 0x94) = ang + 0x8000;
    if (*(u8*)(c + 0x6e3) & 1) {
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    } else {
        *(s16*)(c + 0x8e) = ang;
    }

    if (arg4 == 1) {
        u16 tv = data_ov002_020ff128[*(int*)(c + 8)];
        dmg = (dmg * tv) / 100;
    }
    *(int*)(c + 0x98) = dmg;

    if (arg5 != 0) {
        u8 t = arg5 - 1;
        *(u8*)(((long long)(int)(c + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10;
        *(u8*)(c + 0x6e5) = t;
    }

    func_02014fa4(c + 0x314);
    *(int*)(c + 0x674) = b;

    if (*(u8*)(c + 0x706) == 0) {
        if (*(int*)(c + 0x674) != 0) {
            if (arg6 != 0) {
                volatile struct { int x, y, z; } v;
                int y = *(int*)(c + 0x60);
                int z = *(int*)(c + 0x64);
                int x = *(int*)(c + 0x5c);
                y = y + 0x50000;
                v.x = x;
                v.y = y;
                v.z = z;
                if (*(volatile int*)(c + 0x674) == 1) {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xcb, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xcc, v.x, v.y, v.z);
                } else {
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc9, x, y, z);
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xca, v.x, v.y, v.z);
                }
            }
        }

        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110094);
        if (func_ov002_020d91e0(c, b << 8, 1) != 0) {
            *(u8*)(((long long)(int)(c + 0x6e3)) & 0xFFFFFFFFFFFFFFFFLL) &= 1;
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211010c);
        }
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021100ac);
        func_ov002_020d91e0(c, b << 8, 1);
    }

    return 1;
}
