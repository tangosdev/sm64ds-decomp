typedef struct { int x, y, z; } Vector3;
typedef int Fix12i;

extern char* _ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* m, void* f, int a, Fix12i fix, unsigned sp0);
extern void func_ov002_020aea30(char* c, char* p, int r2);
extern void func_02012694(int a, char* b);
extern void _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(char* c, short* v, char* p);
extern int _ZN8dActor_c24BumpedUnderneathByPlayerER6Player(char* c, char* p);
extern int _ZN6Player9IsOnShellEv(char* p);
extern void _ZN8dActor_c13SmallPoofDustEv(char* c);
extern void _ZN8dActor_c24KillAndTrackInDeathTableEv(char* c);
extern void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(char* c, Vector3* pos, unsigned n, Fix12i fix, short s);
extern int _ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(char* c, char* clsn, char* p);
extern void _ZN6Player6BounceE5Fix12IiE(char* p, Fix12i fix);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* p, Vector3* pos, unsigned a, Fix12i fix, unsigned b, unsigned d, unsigned e);
extern void func_ov065_02117944(char* c, void* p);

extern void* data_ov065_0211d690[];
extern char data_ov065_0211d6e0[];
extern char data_ov065_0211d6f0[];

void func_ov065_0211704c(char* c)
{
    char* p;
    int flags;
    int b;

    if (*(int*)(c + 0x134) == 0) return;
    p = _ZN8dActor_c10FindWithIDEj(*(int*)(c + 0x134));
    if (p == 0) return;
    flags = *(int*)(c + 0x130);
    if (flags & 0x2400) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x10) {
        short v[3];
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        _ZN12dEnemyBase_c20KillByInvincibleCharERK10Vector3_16R6Player5Fix12IiE(c, v, p);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 3;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }

    b = (int)(*(unsigned short*)(p + 0xc) == 0xbf);
    if (b == 0)
        return;

    if (_ZN8dActor_c24BumpedUnderneathByPlayerER6Player(c, p) == 1)
        goto kill;
    if (_ZN6Player9IsOnShellEv(p) == 1)
        goto kill;
    if (*(unsigned char*)(p + 0x6f9) != 1)
        goto jumped;

kill:
    {
        int pos[3];
        _ZN8dActor_c13SmallPoofDustEv(c);
        _ZN8dActor_c24KillAndTrackInDeathTableEv(c);
        func_02012694(0x112, c + 0x74);
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(
            c, (Vector3*)pos, *(unsigned char*)(c + 0x10a) + 1, 0xa000, 0);
        return;
    }

jumped:
    if (_ZN8dActor_c16JumpedOnByPlayerER5dCc_cR6Player(c, c + 0x110, p) != 0) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_ov002_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    {
        int pos[3];
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, (Vector3*)pos, 1, 0xc000, 1, 0, 1);
    }
    if (*(char**)(c + 0x420) != data_ov065_0211d6e0)
        return;
    *(int*)(c + 0x430) = 0;
    func_ov065_02117944(c, data_ov065_0211d6f0);
}
