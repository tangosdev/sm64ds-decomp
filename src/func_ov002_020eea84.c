typedef unsigned short u16;
typedef struct P2 { int a, b; } P2;
typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dBgPi { void *vtb; int s0, s1, s2, s3, s4; u16 f, g; int h, i, j; } dBgPi;

extern int func_02035638(char *p);
extern char *func_0203564c(char *p);
extern unsigned int _ZNK5dBgPi9GetClsnIDEv(void *r);
extern char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, void *v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
extern void _ZN5dBgPiD1Ev(void *r);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(char *p);
extern char *_ZNK10dBgCh_Actr13GetWallResultEv(char *p);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void *v);
extern void *data_02099368[];

int func_ov002_020eea84(char *self, char *player)
{
    dBgPi res;
    Vector3 v;
    Vector3 v2;
    char *actor;
    int b;

    b = (int)(*(u16 *)(player + 0xc) == 0xbf);
    if (b == 0)
        return 0;

    if (func_02035638(self)) {
        char *r = func_0203564c(self);
        int *d = &res.s0;
        *(P2 *)d = *(P2 *)(r + 4);
        d[2] = *(int *)(r + 0xc);
        d[3] = *(int *)(r + 0x10);
        d[4] = *(int *)(r + 0x14);
        res.vtb = data_02099368;
        res.f = *(u16 *)(r + 0x18);
        res.g = *(u16 *)(r + 0x1a);
        res.h = *(int *)(r + 0x1c);
        res.i = *(int *)(r + 0x20);
        res.j = *(int *)(r + 0x24);
        if (_ZNK5dBgPi9GetClsnIDEv(&res) != 0xffffffff) {
            actor = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(&res));
            if (actor != 0 && (b = (int)(*(u16 *)(actor + 0xc) == 0x3a)) != 0) {
                { int *s = (int *)(((int)player + 0x5c)); v.x = s[0]; v.y = s[1]; v.z = s[2]; }
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v, 3, 0xc000, 1, 0, 1);
                _ZN5dBgPiD1Ev(&res);
                return 1;
            }
        }
        _ZN5dBgPiD1Ev(&res);
    }

    if (_ZNK10dBgCh_Actr8IsOnWallEv(self)) {
        char *wr = _ZNK10dBgCh_Actr13GetWallResultEv(self);
        if (_ZNK5dBgPi9GetClsnIDEv(wr) != 0xffffffff) {
            actor = _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(wr));
            if (actor != 0 && (b = (int)(*(u16 *)(actor + 0xc) == 0x139)) != 0) {
                { int *s = (int *)(((int)actor + 0x5c)); v2.x = s[0]; v2.y = s[1]; v2.z = s[2]; }
                if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(player, &v2, 1, 0xc000, 1, 0, 1) != 0)
                    _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, actor + 0x74);
                return 1;
            }
        }
    }
    return 0;
}
