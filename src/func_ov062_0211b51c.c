typedef struct { int x, y, z; } Vector3;
typedef unsigned short u16;
typedef unsigned char u8;
enum Bool { FALSE, TRUE };

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int func_ov062_0211c658(void *c, void *p);
extern int _ZN5Actor24BumpedUnderneathByPlayerER6Player(void *c, void *pl);
extern int _ZN6Player9IsOnShellEv(void *pl);
extern int _ZN6Player15IsCollectingCapEv(void *pl);
extern void _ZN6Player18SetNewHatCharacterEjjb(void *pl, unsigned int a, unsigned int b, int c);
extern int _ZN8SaveData16HasPlayerLostCapEv(void);
extern void _ZN8SaveData13PlayerLoseCapEv(void);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void *pos, void *rot, int e, int f);
extern void _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(void *a, int b, int c, int d, int e);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *pl, Vector3 *v, unsigned int a, int b, unsigned int c, unsigned int d, unsigned int e);
extern char data_ov062_0211e14c[];
extern char data_ov062_0211e17c[];
extern char data_ov062_0211e18c[];

int func_ov062_0211b51c(char *c)
{
    void *pl;
    void *sp;
    Vector3 pos;
    unsigned int flag;
    int fl;
    int id;
    enum Bool isPlayer;
    void *state;
    int r;
    u8 hc;
    u8 st;
    int newchar;
    int zero;

    id = *(int *)(c + 0x134);
    if (id != 0) {
        pl = _ZN5Actor10FindWithIDEj((unsigned int)id);
        if (pl == 0)
            return (int)pl;
        fl = *(int *)(c + 0x130);
        if ((fl & 0x4000) != 0)
            return func_ov062_0211c658(c, data_ov062_0211e14c);
        if ((fl & 0x27f0) != 0)
            return func_ov062_0211c658(c, data_ov062_0211e14c);
        isPlayer = (enum Bool)(*(u16 *)((char *)pl + 0xc) == 0xbf);
        if (isPlayer) {
            if (_ZN5Actor24BumpedUnderneathByPlayerER6Player(c, pl) == 1 ||
                *(u8 *)((char *)pl + 0x6f9) == 1 ||
                _ZN6Player9IsOnShellEv(pl) == 1)
                return func_ov062_0211c658(c, data_ov062_0211e14c);
        }
    }

    id = *(int *)(c + 0x168);
    if (id == 0)
        return id;
    pl = _ZN5Actor10FindWithIDEj((unsigned int)id);
    if (pl == 0)
        return (int)pl;
    isPlayer = (enum Bool)(*(u16 *)((char *)pl + 0xc) == 0xbf);
    if (!isPlayer)
        return (int)isPlayer;
    state = data_ov062_0211e18c;
    if (*(void **)(c + 0x42c) != state)
        return (int)state;
    r = _ZN6Player15IsCollectingCapEv(pl);
    if (r != 0)
        return r;
    r = *(int *)(c + 0x44c);
    if (r != 0)
        return r;

    st = *(u8 *)(c + 0x448);
    hc = *(u8 *)((char *)pl + 0x6d9);
    if (st != 2 &&
        *(u8 *)((char *)pl + 0x6ff) == 0 &&
        *(u8 *)((char *)pl + 0x6fd) == 0 &&
        *(u8 *)((char *)pl + 0x6fb) == 0 &&
        *(u8 *)((char *)pl + 0x6f9) == 0 &&
        *(int *)((char *)pl + 8) != 3) {
        newchar = *(int *)((char *)pl + 8);
        if (hc != newchar) {
            _ZN6Player18SetNewHatCharacterEjjb(pl, hc, 0, 0);
        } else {
            r = _ZN8SaveData16HasPlayerLostCapEv();
            if (r != 0)
                return r;
            _ZN8SaveData13PlayerLoseCapEv();
        }
        {
            int area = *(signed char *)(c + 0xcc);
            unsigned int ch = *(unsigned int *)((char *)pl + 8);
            unsigned int param = 0;
            param = param | (ch << 8);
            sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(
                0x10d, param, c + 0x5c, 0, area, -1);
        }
        if (sp != 0) {
            _ZN5Actor9SetRangesE5Fix12IiES1_S1_S1_(
                sp, 0x64000, 0x258000, 0x1f40000, 0x1f40000);
            *(int *)(c + 0x44c) = *(int *)((char *)sp + 4);
            func_ov062_0211c658(c, data_ov062_0211e17c);
        }
    }

    *(u8 *)(c + 0x447) = 1;
    flag = 0;
    if (*(u8 *)(c + 0x448) == 2 || *(int *)((char *)pl + 8) == 3)
        flag = 1;

    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    r = _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(pl, &pos, flag, 0xc000, 1, 0, 1);
    if (r == 0)
        return r;
    *(u16 *)(c + 0x444) = 0x1e;
    return func_ov062_0211c658(c, data_ov062_0211e17c);
}
