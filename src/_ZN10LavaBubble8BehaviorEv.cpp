//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef int Fix12;
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;

extern "C" {
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, Fix12 d);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
unsigned short DecIfAbove0_Short(unsigned short* p);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void* self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, CylinderClsn* cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, WithMeshClsn* wm, unsigned int j);
void _ZN12CylinderClsn5ClearEv(CylinderClsn* self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn* self);
}

extern "C" int _ZN10LavaBubble8BehaviorEv(char* c)
{
    int flags = *(int*)(c + 0xb0);
    int b20 = (flags & 0x20000) != 0;

    if (b20) {
        M* m = *(M**)(c + 0x300);
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
        return 1;
    }

    {
        int b40 = (flags & 0x40000) != 0;
        if (b40)
            return 1;
    }

    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(c, 0x5dc000)) {
        if (*(u8*)(c + 0x310) != 0)
            _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short*)(c + 0x100));

    {
        unsigned int id = *(unsigned int*)(c + 0x134);
        if (id != 0) {
            if ((*(int*)(c + 0x130) & 0x8000) == 0) {
                char* a = (char*)_ZN5Actor10FindWithIDEj(id);
                if (a != 0) {
                    int hit = (*(u16*)(a + 0xc) == 0xbf);
                    if (hit)
                        _ZN6Player4BurnEv(a);
                }
            } else {
                *(int*)(((long long)(int)(c + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
            }
        }
    }

    {
        M* m = *(M**)(c + 0x300);
        if (m->pmf != 0)
            (((Klass*)c)->*(m->pmf))();
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn*)(c + 0x110));

    if (*(int*)(c + 0x9c) != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn*)(c + 0x144), 0);

    _ZN12CylinderClsn5ClearEv((CylinderClsn*)(c + 0x110));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn*)(c + 0x110));
    return 1;
}
