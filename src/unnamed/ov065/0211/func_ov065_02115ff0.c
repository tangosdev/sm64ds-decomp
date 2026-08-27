extern void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void func_ov002_020aea30(char* c, void* p, int a, int b);
extern void func_02012694(int a, void* b);
extern int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(char* c, void* p);
extern void func_ov065_0211691c(char* c, void* pmf);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(char* c, void* v, void* p, int a);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void func_ov065_02115f84(char* c);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(char* c, void* cyl, void* p);
extern void _ZN6Player6BounceE5Fix12IiE(void* p, int f);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, int a, int b, int d, int e, int f);

extern char data_ov065_0211d660;

void func_ov065_02115ff0(char* c)
{
    short v[3];
    int hv[3];
    char* p;
    int flags;
    unsigned int id;

    id = *(unsigned int*)(c + 0x134);
    if (id == 0) return;
    p = (char*)_ZN8dActor_c10FindWithIDEj(id);
    if (p == 0) return;
    flags = *(int*)(c + 0x130);

    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        func_ov002_020aea30(c, p, 0, 4);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x2400) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(int*)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0, 3);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (_ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, p) == 1) {
        func_02012694(0x11e, c + 0x74);
        func_ov065_0211691c(c, &data_ov065_0211d660);
        return;
    }
    if (flags & 0x10) {
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, p, 0);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x40) {
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0, 2);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    {
        int b = (int)(((long long)(*(unsigned short*)(p + 0xc) == 0xbf)));
        if (b == 0) return;
    }
    if (*(unsigned char*)(p + 0x6f9) == 1 || _ZN6Player9IsOnShellEv(p) == 1) {
        func_ov065_02115f84(c);
        return;
    }
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, p)) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0, 1);
        func_02012694(0x11e, c + 0x74);
        return;
    }
    hv[0] = *(int*)(c + 0x5c);
    hv[1] = *(int*)(c + 0x60);
    hv[2] = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, hv, 2, 0xc000, 1, 0, 1);
}
