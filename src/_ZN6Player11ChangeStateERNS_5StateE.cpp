//cpp
struct C3;
typedef int (C3::*PMF)();

struct State {
    PMF onEnter;
    char pad[8];
    PMF onExit;
};

extern "C" {
extern void func_ov002_020d4540(char *p);
extern void func_ov002_020c9e18(char *c);
extern void func_ov002_020e6780(char *p);
extern void func_0200d81c(void *thiz, int playerID);

extern void *data_0209f318;
extern State data_ov002_0211022c;
extern State data_ov002_0211013c;
extern State data_ov002_0211067c;
extern State data_ov002_021106ac;
extern State data_ov002_02110364;
}

extern "C" int _ZN6Player11ChangeStateERNS_5StateE(char *c, State *newState)
{
    *(State**)(c+0x378) = newState;

    {
        State *cur;
        int ok;
        cur = *(State**)(c+0x370);
        if (cur == 0) { ok = 1; goto check_ok; }
        if (*(int*)((char*)cur+0x10) == 0) { ok = 1; goto check_ok; }
        ok = (((C3*)c)->*(cur->onExit))();
    check_ok:
        if (!ok) return 0;
    }

    if (*(State**)(c+0x370) == &data_ov002_0211022c
        && (unsigned short)(*(unsigned short*)(c+0x6ce) & 0x400) == 0
        && *(unsigned char*)(c+0x709) != 0
        && newState != &data_ov002_0211013c
        && newState != &data_ov002_0211067c
        && newState != &data_ov002_021106ac
        && newState != &data_ov002_0211022c)
    {
        return 0;
    }

    {
        int b = (*(int*)(c+0x358) != 0);
        if (b) {
        if (newState == &data_ov002_0211013c) {
            *(unsigned char*)(c+0x6e3) = 1;
            newState = &data_ov002_02110364;
        }
        }
    }

    *(int*)(c+0x620) = 0;
    *(int*)(c+0x630) = 0;
    *(int*)(c+0x62c) = *(int*)(c+0x630);
    *(int*)(c+0x628) = *(int*)(c+0x62c);

    *(State**)(c+0x374) = *(State**)(c+0x370);
    *(State**)(c+0x370) = newState;

    *(unsigned char*)(c+0x717) = 0;
    *(unsigned char*)(c+0x713) = 1;
    *(unsigned char*)(c+0x716) = 0;

    *(int*)(c+0x694) = 0;
    *(int*)(c+0x690) = *(int*)(c+0x694);

    *(unsigned char*)(c+0x712) = 0;
    *(int*)(c+0x654) = 0;

    *(short*)(c+0x90) = 0;
    *(short*)(c+0x8c) = *(short*)(c+0x90);

    *(unsigned char*)(c+0x6f6) = 0;
    *(unsigned char*)(c+0x6ec) = 0;
    *(unsigned char*)(c+0x726) = 0;

    *(unsigned short*)(((long long)(int)(c+0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x200;

    {
        int v1 = 0x4000;
        int v2 = 0x4b000;
        v1 = -v1;
        v2 = -v2;
        *(int*)(c+0x9c) = v1;
        *(int*)(c+0xa0) = v2;
    }

    *(short*)(c+0x6a4) = 0;

    func_ov002_020d4540(c);

    *(unsigned char*)(c+0x708) = 0;
    func_ov002_020c9e18(c);

    func_ov002_020e6780(c);

    if (data_0209f318 != 0) {
        func_0200d81c(data_0209f318, *(unsigned char*)(c+0x6d8));
    }

    {
        State *s = *(State**)(c+0x370);
        if (*(int*)s == 0) return 1;
        return (((C3*)c)->*(s->onEnter))();
    }
}
