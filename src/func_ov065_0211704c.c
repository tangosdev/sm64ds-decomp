extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(char* m, void* f, int a, int fix, unsigned sp0);
extern void func_020aea30(char* c, char* p, int r2);
extern void func_02012694(int a, char* b);
extern void func_020ada40(char* c, short* v, char* p);
extern int _ZN5Actor24BumpedUnderneathByPlayerER6Player(char* c, char* p);
extern int _ZN6Player9IsOnShellEv(char* p);
extern void _ZN5Actor13SmallPoofDustEv(char* c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(char* c);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(char* c, int* pos, unsigned n, int fix, short s);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(char* c, char* clsn, char* p);
extern void _ZN6Player6BounceE5Fix12IiE(char* p, int fix);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* p, int* pos, unsigned a, int fix, unsigned b, unsigned d, unsigned e);
extern void func_ov065_02117944(char* c, void* p);

extern void* data_ov065_0211d690[];
extern int data_ov065_0211d6e0;
extern void* data_ov065_0211d6f0;

void func_ov065_0211704c(char* c)
{
    char* p;
    int flags;
    if (*(int*)(c + 0x134) == 0) return;
    p = _ZN5Actor10FindWithIDEj(*(int*)(c + 0x134));
    if (p == 0) return;
    flags = *(int*)(c + 0x130);
    if (flags & 0x2400) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x10) {
        short v[3];
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        func_020ada40(c, v, p);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40000) {
        *(int*)(c + 0x10c) = 4;
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x4380) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 3;
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x40) {
        *(unsigned char*)(c + 0x43c) = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, data_ov065_0211d690[1], 0, 0x1000, 0);
        *(int*)(c + 0x10c) = 2;
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    if (flags & 0x20) {
        *(int*)(c + 0x10c) = 1;
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    {
    int b = (*(unsigned short*)(p + 0xc) == 0xbf) ? 1 : 0;
    if ((int)(((long long)b) & 0xFFFFFFFFFFFFFFFFLL) == 0) return;
    {
    if (_ZN5Actor24BumpedUnderneathByPlayerER6Player(c, p) == 1) goto poof;
    if (_ZN6Player9IsOnShellEv(p) == 1) goto poof;
    if (*(unsigned char*)(p + 0x6f9) == 1) {
        int pos[3];
poof:;
        _ZN5Actor13SmallPoofDustEv(c);
        _ZN5Actor24KillAndTrackInDeathTableEv(c);
        func_02012694(0x112, c + 0x74);
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, pos, *(unsigned char*)(c + 0x10a) + 1, 0xa000, 0);
        return;
    }
    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x110, p) != 0) {
        _ZN6Player6BounceE5Fix12IiE(p, 0x28000);
        *(int*)(c + 0x10c) = 1;
        func_020aea30(c, p, 0);
        func_02012694(0x112, c + 0x74);
        return;
    }
    {
        int pos[3];
        pos[0] = *(int*)(c + 0x5c);
        pos[1] = *(int*)(c + 0x60);
        pos[2] = *(int*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, pos, 1, 0xc000, 1, 0, 1);
    }
    if (*(int*)(c + 0x420) != (int)&data_ov065_0211d6e0) return;
    *(int*)(c + 0x430) = 0;
    func_ov065_02117944(c, &data_ov065_0211d6f0);
    }
    }
}
