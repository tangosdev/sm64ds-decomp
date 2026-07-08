typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern s8 LEVEL_ID;
extern int data_ov084_02130cc8[];
extern int data_ov084_02130d74[];
extern int data_ov084_02130228[];

extern void func_ov084_02129864(char* c);
extern void func_ov084_021290d4(char* c);
extern int _ZN8CapEnemy11GetCapStateEv(char* c);
extern void _ZN5Actor8PoofDustEv(char* c);
extern int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(char* c, Fix12 f);
extern void _ZN8CapEnemy12Unk_02005d94Ev(char* c);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(char* c, void* w, void* m, u32 j);
extern void func_ov084_02129498(char* c);
extern void _ZN8CapEnemy10ReleaseCapERK7Vector3(char* c, Vector3* v);
extern void _ZN8CapEnemy15RespawnIfHasCapEv(char* c);
extern void func_ov084_021296cc(char* c);
extern int func_ov084_021298d0(char* c);
extern void func_ov084_0212a580(char* c);
extern int func_ov084_02129a00(char* c);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* c, void* cyl);
extern void func_ov084_0212934c(char* c);
extern void _ZN9Animation7AdvanceEv(void* c);
extern void func_ov084_02129ed4(void* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* c, void* cyl);
extern int _ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(char* c, void* w, Fix12 f, int s, int b1, int b2, Fix12 g);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(char* c, void* w, u32 j);
extern void func_ov084_021294d0(char* c);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void func_ov084_02129238(char* c);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void func_ov084_02129c9c(char* c);
extern void _ZN5Enemy9SpawnCoinEv(char* c);

int func_ov084_0212b6ec(char* c)
{
    Vector3 v1;
    Vector3 v2;
    int r;
    int st;

    func_ov084_02129864(c);
    func_ov084_021290d4(c);
    r = _ZN8CapEnemy11GetCapStateEv(c);
    if (r == 0)
        return 1;
    if (r == 1) {
        *(u32*)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10000000;
        _ZN5Actor8PoofDustEv(c);
    }
    if (*(s32*)(c + 0x460) != 3 && *(s32*)(c + 0x434) != 3 &&
        *(u8*)(c + 0x107) == 0 && *(s32*)(c + 0x10c) == 0 &&
        _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000) != 0)
    {
        _ZN8CapEnemy12Unk_02005d94Ev(c);
        return 1;
    }

    if (*(s32*)(c + 0x10c) != 0) {
        r = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, c + 0x1b4, c + 0x370, 3);
        if (r != 0) {
            if (r == 2) {
                func_ov084_02129498(c);
                v1.x = 0;
                v1.y = 0x6c000;
                v1.z = 0;
                _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v1);
                *(s32*)(c + 0x5c) = *(s32*)(c + 0x41c);
                *(s32*)(c + 0x60) = *(s32*)(c + 0x420);
                *(s32*)(c + 0x64) = *(s32*)(c + 0x424);
                *(s16*)(c + 0x8c) = 0;
                *(s16*)(c + 0x8e) = 0;
                *(s16*)(c + 0x90) = 0;
                _ZN8CapEnemy15RespawnIfHasCapEv(c);
                func_ov084_021296cc(c);
            }
            return 1;
        }
        if (func_ov084_021298d0(c) == 0)
            func_ov084_0212a580(c);
        return 1;
    }

    if (func_ov084_02129a00(c) != 0)
        return 1;

    if (*(s32*)(c + 0x434) >= 3 ||
        (*(s32*)(c + 0x460) == 3 && *(s32*)(c + 0x3d0) == data_ov084_02130cc8[1]))
    {
        *(s32*)(c + 0x3cc) = 0x1000;
    } else {
        int v = _ZN4cstd4fdivEii(*(s32*)(c + 0x98), *(s32*)(c + 0x80) * 2);
        if (v > 0x3000)
            v = 0x3000;
        *(s32*)(c + 0x3cc) = v;
    }

    _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c + 0x180);

    if (*(s32*)(c + 0x434) != 2) {
        func_ov084_0212934c(c);
        _ZN9Animation7AdvanceEv(c + 0x3c0);
    }

    st = *(s32*)(c + 0x434);
    {
        int* q = &data_ov084_02130d74[st * 2];
        int adj = q[1];
        char* thiz = c + (adj >> 1);
        void (*fn)(char*);
        if (adj & 1)
            fn = *(void(**)(char*))(*(char**)thiz + q[0]);
        else
            fn = (void(*)(char*))q[0];
        fn(thiz);
    }

    {
        u16* hp = (u16*)(c + 0x100);
        *hp += 1;
        if (st != *(s32*)(c + 0x434))
            *hp = 0;
    }

    func_ov084_02129ed4(c);

    if (*(u8*)(c + 0x113) < 6)
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    else
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x180);

    if (*(s32*)(c + 0x10c) == 0 && *(s32*)(c + 0x434) != 2 && *(s32*)(c + 0x434) != 3) {
        if (_ZN5Enemy15IsGoingOffCliffER12WithMeshClsn5Fix12IiEsbbS3_(c, c + 0x1b4, 0x32000, 0x1f49, 0, 1, 0x32000) != 0) {
            *(s32*)(c + 0x5c) = *(s32*)(c + 0x410);
            *(s32*)(c + 0x60) = *(s32*)(c + 0x414);
            *(s32*)(c + 0x64) = *(s32*)(c + 0x418);
        } else {
            *(s32*)(c + 0x410) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x414) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x418) = *(s32*)(c + 0x64);
        }
    }

    {
        int lvl = *(s32*)(c + 0x460);
        if (lvl == 0) {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x1b4, 0);
        } else if (LEVEL_ID == 6 || LEVEL_ID == 0x1b) {
            if (*(s32*)(c + 0x444) == data_ov084_02130228[lvl] && *(s32*)(c + 0x10c) != 7)
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x1b4, 3);
            else
                _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x1b4, 2);
        } else {
            _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, c + 0x1b4, 2);
        }
    }

    func_ov084_021294d0(c);
    _ZN12CylinderClsn5ClearEv(c + 0x180);
    if (*(s32*)(c + 0x10c) == 0)
        _ZN12CylinderClsn6UpdateEv(c + 0x180);
    func_ov084_0212a580(c);
    func_ov084_02129238(c);

    if (*(s32*)(c + 0x434) == 0) {
        int b = (*(s32*)(c + 0xb0) & 8) ? 1 : 0;
        if (b == 0) {
            if (Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x428)) < 0xa000) {
                *(u16*)(((long long)(int)(c + 0x456)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
                if (*(u8*)(c + 0x113) < 6 && *(u16*)(c + 0x456) == 0x1e) {
                    func_ov084_02129c9c(c);
                    *(u16*)(c + 0x458) = 0x5a;
                }
                if (*(u16*)(c + 0x456) >= 0x12c && *(u16*)(c + 0x458) == 0) {
                    func_ov084_021296cc(c);
                    _ZN5Enemy9SpawnCoinEv(c);
                    func_ov084_02129498(c);
                    v2.x = 0;
                    v2.y = 0x6c000;
                    v2.z = 0;
                    _ZN8CapEnemy10ReleaseCapERK7Vector3(c, &v2);
                    *(s32*)(c + 0x5c) = *(s32*)(c + 0x41c);
                    *(s32*)(c + 0x60) = *(s32*)(c + 0x420);
                    *(s32*)(c + 0x64) = *(s32*)(c + 0x424);
                    _ZN8CapEnemy15RespawnIfHasCapEv(c);
                    return 1;
                }
            } else {
                *(u16*)(c + 0x456) = 0;
                *(s32*)(c + 0x428) = *(s32*)(c + 0x5c);
                *(s32*)(c + 0x42c) = *(s32*)(c + 0x60);
                *(s32*)(c + 0x430) = *(s32*)(c + 0x64);
            }
            goto done;
        }
    }

    if (*(u16*)(c + 0x458) == 0)
        *(u16*)(c + 0x456) = 0;
done:
    return 1;
}
