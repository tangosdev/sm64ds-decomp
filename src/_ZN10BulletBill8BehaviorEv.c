//cpp
// NONMATCHING: missing logic (ROM does more) (div=2). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int Fix12;

struct Klass; typedef void (Klass::*PMF)();
struct M { PMF pmf; };

struct Vector3 { int x, y, z; };

extern "C" {
void func_0200f760(void *self, void *cc);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *cc);
void *_ZN5Actor10FindWithIDEj(u32 id);
void _ZN6Player16IncMegaKillCountEv();
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32, int, int, int);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void func_02012694(int a, void *b);
void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, const Vector3 *pos);
void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const Vector3 *pos, u32 a, Fix12 b, u32 c, u32 d, u32 e);
void func_ov079_02126704(char *c);
void _ZN12CylinderClsn5ClearEv(void *self);
void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *pos);
void _ZN12CylinderClsn6UpdateEv(void *self);

extern M data_ov079_021282e0[];
}

extern "C" int _ZN10BulletBill8BehaviorEv(char *c)
{
    int flags;
    u32 which;
    u32 id;

    func_0200f760(c, c + 0x110);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    which = *(u32 *)(c + 0x3d4);
    {
        M *m = &data_ov079_021282e0[which];
        (((Klass *)c)->*(m->pmf))();
    }

    id = *(u32 *)(c + 0x134);
    if (id != 0) {
        flags = *(int *)(c + 0x130);
        if (flags & 0x10) {
            _ZN5Actor10FindWithIDEj(id);
            _ZN6Player16IncMegaKillCountEv();
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8f, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            _ZN9ActorBase18MarkForDestructionEv(c);
            func_02012694(0x78, c + 0x74);
        } else if (flags & 0x3c0) {
            *(int *)(c + 0x3d4) = 1;
            {
                Vector3 *pos = (Vector3 *)(c + 0x74);
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, pos);
            }
        } else {
            void *o = _ZN5Actor10FindWithIDEj(id);
            if (o != 0) {
                int eq = (*(u16 *)((char *)o + 0xc) == 0xbf);
                if (eq != 0) {
                    if (*(u8 *)((char *)o + 0x6fb) == 0) {
                        Vector3 pos;
                        pos.x = *(int *)(c + 0x5c);
                        pos.y = *(int *)(c + 0x60);
                        pos.z = *(int *)(c + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(o, &pos, 3, 0xc000, 1, 0, 1);
                        *(int *)(c + 0x3d4) = 1;
                        {
                            Vector3 *p2 = (Vector3 *)(c + 0x74);
                            _ZN5Sound9PlayBank0EjRK7Vector3(0xb5, p2);
                        }
                    }
                }
            }
        }
    }

    {
        u16 *h = (u16 *)(c + 0x100);
        *h = *h + 1;
        if (which != *(u32 *)(c + 0x3d4)) {
            *h = 0;
        }
    }

    func_ov079_02126704(c);

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        Vector3 pos;
        pos.x = 0;
        pos.y = -0x50000;
        pos.z = 0;
        _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(c + 0x110, &pos);
    }
    _ZN12CylinderClsn6UpdateEv(c + 0x110);

    *(short *)(c + 0x92) = *(short *)(c + 0x8c);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    *(short *)(c + 0x96) = *(short *)(c + 0x90);
    return 1;
}
