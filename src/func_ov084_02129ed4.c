typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

typedef struct { s32 x, y, z; } Vector3;

extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN8CapEnemy10ReleaseCapERK7Vector3(void* thiz, const Vector3* v);
extern void* func_020ada40(void* thiz, s16* v, void* r6, s32 flag);
extern void func_02012694(s32 a, void* b);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thiz, void* f, s32 i, s32 fx, u32 j);
extern s32 _ZN6Player9IsOnShellEv(void* p);
extern s32 _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* thiz, void* clsn, void* player);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, s32 f);
extern void _ZN5Actor13SmallPoofDustEv(void* thiz);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, s32 f, u32 b, u32 cc, u32 d);
extern void func_ov084_02129498(void* thiz);
extern void func_020aea30(void* thiz, void* r6, void* p);

extern void* data_ov084_02130cd0[];
extern void* data_ov084_02130ce0[];
extern u8 data_ov084_02130204[];

#define I(p,o)    (*(s32*)((char*)(p)+(o)))
#define U16f(p,o) (*(u16*)((char*)(p)+(o)))
#define S16f(p,o) (*(s16*)((char*)(p)+(o)))
#define U8f(p,o)  (*(u8*)((char*)(p)+(o)))

void func_ov084_02129ed4(void* c)
{
    s16 aC[3];
    s16 a12[3];
    s16 a18[3];
    volatile Vector3 v20;
    volatile Vector3 v2c;
    Vector3 v38;
    Vector3 v44;
    Vector3 v50;
    Vector3 v5c;
    Vector3 v68;
    Vector3 v74;
    void* r6;
    u32 flags;
    s32 var_r5;
    s32 var_r4;
    u16 t2;
    s32 var_r1;
    s32 var_r0;
    u32 id;

    id = *(u32*)((char*)c + 0x1a4);
    if (id == 0) return;
    r6 = _ZN5Actor10FindWithIDEj(id);
    if (r6 == 0) return;

    flags = I(c, 0x1a0);
    var_r4 = 0xc000;
    I(c, 0x46c) = flags;
    t2 = U16f(c, 0xc);
    var_r5 = 0;
    if (I(c, 0x460) == 3) var_r4 = 0x5000;
    var_r1 = (s32)(t2 == 0xc9);

    if (var_r1 == 0 && (flags & 0x10)) {
        v38.x = 0; v38.y = 0x6c000; v38.z = 0;
        _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v38);
        var_r0 = (s32)(U16f(c, 0xc) == 0xc8);
        if (var_r0 != 0) {
            aC[0] = -0x2000; aC[1] = 0; aC[2] = 0;
            func_020ada40(c, aC, r6, 0x41000);
            return;
        }
        a12[0] = -0x1800; a12[1] = 0; a12[2] = 0;
        func_020ada40(c, a12, r6, 0x96000);
        return;
    }

    if (flags & 0x20) {
        I(c, 0x10c) = 1;
        if (I(c, 0x460) == 2) U8f(c, 0x108) = 3;
        I(c, 0x80) = 0x1000;
        I(c, 0x84) = 0x1000;
        I(c, 0x88) = 0x1000;
        func_02012694(0xe0, (char*)c + 0x74);
        goto block_68;
    }

    if (flags & 0x40000) {
        var_r5 = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
        I(c, 0x10c) = 4;
        goto block_68;
    }

    var_r1 = (s32)(t2 == 0xca);
    if (var_r1 == 0) {
        if (flags & 0x20000) {
            I(c, 0x10c) = 7;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            *(s32*)(((int)c + 0x198) & 0xffffffffffffffffULL) |= 1;
            goto block_68;
        }
        if (flags & 0x2400) {
            I(c, 0x10c) = 5;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x4000) {
            I(c, 0x10c) = 6;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            goto block_68;
        }
        if (flags & 0x380) {
            var_r5 = 1;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130cd0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 3;
            goto block_68;
        }
        if (flags & 0x40) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
            I(c, 0x10c) = 2;
            var_r5 = 1;
            goto block_68;
        }
        if (!(flags & 0x8000)) {
            var_r0 = (s32)(U16f(r6, 0xc) == 0xbf);
            if (var_r0 != 0) {
                if (U8f(r6, 0x6f9) != 0) {
                    v44.x = 0; v44.y = 0x6c000; v44.z = 0;
                    _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v44);
                    a18[0] = 0x2000; a18[1] = 0; a18[2] = 0;
                    func_020ada40(c, a18, r6, 0x41000);
                    return;
                }
                { Vector3* pp = (Vector3*)(((int)r6 + 0x5c) & 0xffffffffffffffffULL); v20.x = pp->x; v20.y = pp->y; v20.z = pp->z; }
                if (_ZN6Player9IsOnShellEv(r6) != 0) {
                    I(c, 0x10c) = 5;
                    var_r5 = 1;
                    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((char*)c + 0x370, data_ov084_02130ce0[1], 0x40000000, 0x1000, 0);
                    goto block_68;
                }
                if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, (char*)c + 0x180, r6) != 0) {
                    _ZN6Player6BounceE5Fix12IiE(r6, 0x28000);
                    func_02012694(0xe0, (char*)c + 0x74);
                    I(c, 0x10c) = 1;
                    I(c, 0x80) = 0x1000;
                    I(c, 0x84) = 0x1000;
                    I(c, 0x88) = 0x1000;
                    goto block_68;
                }
                if (U8f(r6, 0x6fb) != 0) return;
                if (I(c, 0x434) == 0) {
                    if (I(c, 0x460) == 0) {
                        _ZN5Actor13SmallPoofDustEv(c);
                        v50.x = I(c, 0x5c); v50.y = I(c, 0x60); v50.z = I(c, 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r6, &v50, 0, var_r4, 1, 0, 1);
                        func_ov084_02129498(c);
                        func_02012694(0x110, (char*)c + 0x74);
                        return;
                    }
                    if ((I(c, 0x1a0) & 0x400000) == 0) return;
                    v5c.x = I(c, 0x5c); v5c.y = I(c, 0x60); v5c.z = I(c, 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r6, &v5c, data_ov084_02130204[I(c, 0x460)], var_r4, 1, 0, 1);
                    I(c, 0x434) = 1;
                    return;
                }
                goto block_68;
            }
            goto block_68;
        }
        goto block_68;
    }

    var_r0 = (s32)(U16f(r6, 0xc) == 0xbf);
    if (var_r0 != 0) {
        { Vector3* pp = (Vector3*)(((int)r6 + 0x5c) & 0xffffffffffffffffULL); v2c.x = pp->x; v2c.y = pp->y; v2c.z = pp->z; }
        if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, (char*)c + 0x180, r6) != 0) {
            _ZN6Player6BounceE5Fix12IiE(r6, 0x28000);
            func_02012694(0xe0, (char*)c + 0x74);
            I(c, 0x10c) = 1;
            I(c, 0x80) = 0x1000;
            I(c, 0x84) = 0x1000;
            I(c, 0x88) = 0x1000;
            goto block_68;
        }
        if (U8f(r6, 0x6fb) != 0) return;
        if (I(c, 0x434) == 0) {
            I(c, 0x434) = 1;
            if ((I(c, 0x1a0) & 0x400000) == 0) return;
            v68.x = I(c, 0x5c); v68.y = I(c, 0x60); v68.z = I(c, 0x64);
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r6, &v68, data_ov084_02130204[I(c, 0x460)], var_r4, 1, 0, 1);
            return;
        }
        goto block_68;
    }

block_68:
    if (I(c, 0x10c) != 0) {
        v74.x = 0; v74.y = 0x6c000; v74.z = 0;
        _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v74);
    }
    func_020aea30(c, r6, (char*)c + 0x1b4);
    if (var_r5 != 0) {
        S16f(c, 0x8e) = (s16)(S16f(c, 0x94) + 0x8000);
    }
    if (I(c, 0x460) != 3) return;
    if ((I(c, 0x46c) & 0x40) || (I(c, 0x46c) & 0x380)) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += I(r6, 0x98);
    }
    if (I(c, 0x46c) & 0x400) {
        *(s32*)(((int)c + 0x98) & 0xffffffffffffffffULL) += 0x20;
    }
}
