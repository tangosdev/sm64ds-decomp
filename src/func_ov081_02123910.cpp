//cpp
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int m29(); /* slot 0x74 = index 29 */
};

extern "C" {
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(void* self, Vector3_16* v, void* player, int unused);
extern void func_ov081_021237ec(void* c);
extern int _ZN6Player9IsOnShellEv(void* p);
extern int func_ov002_020e10a8(void* p);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* self, void* cyl, void* player);
extern void _ZN6Player10SpinBounceE5Fix12IiE(void* p, int f);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, unsigned n, int f, unsigned a, unsigned b, unsigned c);
}

extern "C" void func_ov081_02123910(char* c)
{
    void* other;
    unsigned int id;
    int flags;
    int isPlayer;
    Vector3_16 kv;
    Vector3 pos;

    id = *(unsigned int*)(c + 0x1c0);
    if (id == 0)
        return;
    other = _ZN5Actor10FindWithIDEj(id);
    if (other == 0)
        return;

    flags = *(int*)(c + 0x1bc);
    if ((flags & 0x10) != 0) {
        int tmp;
        kv.x = (short)-0x2000;
        kv.y = 0;
        kv.z = 0;
        tmp = ((VObj*)c)->m29();
        _ZN5Enemy20KillByInvincibleCharERK10Vector3_16R6Player(c, &kv, other, tmp);
        return;
    }

    if ((flags & 0x26000) != 0) {
        func_ov081_021237ec(c);
        return;
    }

    isPlayer = (int)(*(unsigned short*)((char*)other + 0xc) == 0xbf);
    if (isPlayer == 0)
        return;

    if (*(unsigned char*)((char*)other + 0x6f9) == 0) {
        if (_ZN6Player9IsOnShellEv(other) == 0) {
            flags = *(int*)(c + 0x1bc);
            if ((flags & 0x40000) == 0)
                goto cont;
        }
    }
    func_ov081_021237ec(c);
    return;

cont:
    if ((flags & 0x26fe0) != 0) {
        if (func_ov002_020e10a8(other) == 0) {
            func_ov081_021237ec(c);
            return;
        }
    }

    if (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x19c, other) != 0) {
        _ZN6Player10SpinBounceE5Fix12IiE(other, 0x28000);
        func_ov081_021237ec(c);
        return;
    }

    if (*(unsigned char*)((char*)other + 0x6fb) != 0)
        return;
    if ((*(int*)(c + 0x1bc) & 0x400000) == 0)
        return;

    {
        unsigned char one = 1;
        int a;
        short* p;
        *(unsigned char*)(c + 0x39a) = one;
        a = 0xa000;
        p = (short*)(c + 0x100);
        *p = 0;
        *(int*)(c + 0x98) = -a;
    }
    *(short*)(c + 0x94) = Vec3_HorzAngle(c + 0x5c, (char*)other + 0x5c);

    pos.x = *(int*)(c + 0x5c);
    pos.y = *(int*)(c + 0x60);
    pos.z = *(int*)(c + 0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(other, &pos, 2, 0xc000, 1, 0, 1);
}
