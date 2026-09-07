// Matched byte-identical (mwccarm 1.2/sp2p3). Keys: volatile src pointer pins
// the pool struct copy to ldr x,y,z + batched str z,x,y (not ldm/stm); same-type
// u32 reads CSE the c+0x134 load into the FindWithID arg; laundered flags def
// rotates the p/flags r5/r4 coloring; ternary + laundered test materializes the
// ==0xbf bool (moveq #1/movne #0/cmp) instead of jump-threading.
extern void _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(void* thiz, void* v);
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void func_ov002_020aea30(char* c, void* p, int a, int b);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(char* c, void* cyl, void* p);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(char* c, void* v, void* p, int a);
extern void func_02012694(int a, void* b);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(char* c, void* v, unsigned int a, int b, short d);
extern void _ZN8dActor_c8PoofDustEv(char* c);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(char* c);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, int a, int b, int d, int e, int f);

struct V3 { int x, y, z; };
extern struct V3 data_ov090_0213412c;

void func_ov090_021310b4(char* c)
{
    short hv[3];
    struct V3 sv;
    struct V3 cv;
    struct V3 hurt;
    int flags;
    char* p;
    int x, y, z;
    volatile struct V3* src = &data_ov090_0213412c;

    x = src->x;
    y = src->y;
    z = src->z;
    sv.z = z;
    sv.x = x;
    sv.y = y;
    _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3(c + 0x110, &sv);

    if (*(unsigned int*)(c + 0x134) == 0) return;
    if ((p = (char*)_ZN8dActor_c10FindWithIDEj(*(unsigned int*)(c + 0x134))) == 0) return;
    flags = (int)(((long long)*(int*)(c + 0x130)));

    if (flags & 0x2400) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        return;
    }
    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        func_ov002_020aea30(c, p, 0, 4);
        return;
    }
    if (flags & 0x40) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        return;
    }
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x9c) = 0;
        *(int*)(c + 0xa4) = 0;
        *(int*)(c + 0xa8) = 0;
        *(int*)(c + 0xac) = 0;
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        return;
    }
    if (flags & 0x10) {
        hv[0] = 0x1000;
        hv[1] = 0;
        hv[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, hv, p, 0);
        func_02012694(0x1d, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(unsigned char*)(c + 0x3a1) = 3;
        *(int*)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0, 3);
        return;
    }
    {
        int b = (*(unsigned short*)(p + 0xc) == 0xbf) ? 1 : 0;
        if ((int)(((long long)b)) == 0) return;
    }
    if (*(unsigned char*)(p + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(p) == 1) {
        cv.x = *(int*)(c + 0x5c);
        cv.y = *(int*)(c + 0x60);
        cv.z = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &cv, *(unsigned char*)(c + 0x10a) + 1, 0xa000, 0);
        _ZN8dActor_c8PoofDustEv(c);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        return;
    }
    hurt.x = *(int*)(c + 0x5c);
    hurt.y = *(int*)(c + 0x60);
    hurt.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, &hurt, 2, 0xc000, 1, 0, 1);
}
