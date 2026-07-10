//cpp
// NONMATCHING: size 0x1dc vs 0x200 (delta -36); angle-add pointer shape and stack frame (0x18 vs 0x24) residuals
struct Vector3 { int x, y, z; };
extern "C" {
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
void _ZN5Actor14TriplePoofDustEv(void *self);
void _ZN9ActorBase18MarkForDestructionEv(void *self);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *pos);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
int func_02037e38(void *p);
int func_02037e84(void *p);
void func_0203568c(void *p, int v);
void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, Vector3 *pos, int fix);
void Particle_System_NewSimple(unsigned int a, int b, int cc, int d);
void func_020383fc(void *self);
void func_02012694(int a, void *pos);
}
extern "C" void func_ov092_021311b0(void *thiz)
{
    char *c = (char *)thiz;
    int saved[3];
    char *fr;
    Vector3 v;

    {
        short *ip = (short *)(c + 0x8c);
        short *base = (short *)(c + 0x400);
        short *lr = (short *)(c + 0x8e);
        short *r5 = (short *)(c + 0x90);
        *ip = (short)(*ip + base[0xe0/2]);
        *lr = (short)(*lr + base[0xe2/2]);
        *r5 = (short)(*r5 + base[0xe4/2]);
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);

    saved[0] = *(int *)(c + 0x5c);
    saved[1] = *(int *)(c + 0x60);
    saved[2] = *(int *)(c + 0x64);
    func_020383fc(c + 0x324);

    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x324) != 0) {
        _ZN5Actor14TriplePoofDustEv(c);
        _ZN9ActorBase18MarkForDestructionEv(c);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
        return;
    }

    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x324) == 0)
        return;

    *(int *)(c + 0x55c) = *(int *)(c + 0x60);
    fr = (char *)_ZNK12WithMeshClsn14GetFloorResultEv(c + 0x324);

    if (func_02037e38(fr + 4) == 9) {
        int z;
        *(int *)(c + 0x568) = 8;
        z = 0;
        *(int *)(c + 0x98) = z;
        *(int *)(c + 0xa8) = z;
        func_02012694(0x178, c + 0x74);
    } else if (func_02037e84(fr + 4) == 8) {
        char *o;
        int *p;
        *(int *)(c + 0x568) = 7;
        func_0203568c(c + 0x324, 0x78000);
        o = *(char **)(c + 0x320);
        p = (int *)(o + 0x5c);
        v.x = p[0];
        v.y = p[1];
        v.z = p[2];
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, 0xff8000);
        *(int *)(c + 0x5c) = saved[0];
        *(int *)(c + 0x60) = saved[1];
        *(int *)(c + 0x64) = saved[2];
        func_02012694(0x178, c + 0x74);
    } else {
        char *o;
        int *p;
        _ZN5Actor14TriplePoofDustEv(c);
        Particle_System_NewSimple(0xfa, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
        _ZN9ActorBase18MarkForDestructionEv(c);
        o = *(char **)(c + 0x320);
        p = (int *)(o + 0x5c);
        v.x = p[0];
        v.y = p[1];
        v.z = p[2];
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &v, 0xff8000);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c + 0x74);
    }
    *(int *)(c + 0x320) = 0;
}
