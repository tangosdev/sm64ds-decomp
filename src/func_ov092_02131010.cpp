//cpp
// NONMATCHING: div=2 (2/104 words). Sole residual is one scheduler slot: the ROM
// emits "sub r3; mov r2,#1; str ip,[sp,#8]" where every C spelling emits
// "sub r3; str ip,[sp,#8]; mov r2,#1" (LandingDustAt's bool arg vs the volatile
// tmp.z store, adjacent independent insns).
// Tried (still div=2): named/inlined bool, y-reassign/y-overwrite-as-bool, comma-arg
// hoist, b?z:z / z+(b-b) / z|(b&0) fake deps, bool proto, C++ copy-temp / ctor,
// cast-away volatile, partial-volatile fields, equal-arm ternary on dust/bool args
// (6o), address-escape of b, #pragma opt_propagation off, statement/decl order
// perms, 25-min+ permuter (score stuck at 60 = one reorder). Everything else
// byte-exact incl. frame 0x28, pool (0x504, 0x5dc000), and coloring.
// Compiler: mwccarm 1.2/sp2p3. Addr: 0x02131010 size 0x1a0 (ov092).
struct Vector3 { int x, y, z; };
typedef short s16;
#define LA(p) (((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL)
extern "C" {
void *_ZN5Actor13ClosestPlayerEv(void *self);
int func_ov002_020de328(void);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
void func_020383fc(void *a);
int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void *self, const Vector3 *v, int fix);
void func_02012694(int a, int *t);
void _ZN5Actor13LandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
}
extern "C" int func_ov092_02131010(unsigned char *thiz)
{
    volatile Vector3 tmp;
    Vector3 eq;
    Vector3 dust;

    if (_ZN5Actor13ClosestPlayerEv(thiz) != 0) {
        if (func_ov002_020de328() != 0) {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)LA(thiz + 0x500);
            *p504 |= 0x400000;
            *p500 |= 4;
        } else {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)LA(thiz + 0x500);
            *p504 &= ~0x400000;
            *p500 &= ~4;
        }
    }

    {
        s16 *p8c = (s16 *)LA(thiz + 0x8c);
        s16 *p8e = (s16 *)LA(thiz + 0x8e);
        s16 *p90 = (s16 *)LA(thiz + 0x90);
        *p8c += ((s16 *)(thiz + 0x400))[0xe0 / 2];
        *p8e += ((s16 *)(thiz + 0x400))[0xe2 / 2];
        *p90 += ((s16 *)(thiz + 0x400))[0xe4 / 2];
    }

    {
        int v = *(int *)(thiz + 0x9c);
        if (v == 0) return v;
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(thiz, 0);
    func_020383fc(thiz + 0x324);
    {
        int g = _ZNK12WithMeshClsn10IsOnGroundEv(thiz + 0x324);
        if (g == 0) return g;
    }

    {
        short lr = 0;
        ((short *)(thiz + 0x400))[0xe4 / 2] = lr;
        ((short *)(thiz + 0x400))[0xe2 / 2] = ((short *)(thiz + 0x400))[0xe4 / 2];
        ((short *)(thiz + 0x400))[0xe0 / 2] = ((short *)(thiz + 0x400))[0xe2 / 2];
        *(int *)(thiz + 0x98) = lr;
        *(int *)(thiz + 0xa8) = lr;
    }

    eq.x = *(int *)(thiz + 0x5c);
    eq.y = *(int *)(thiz + 0x60);
    eq.z = *(int *)(thiz + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(thiz, &eq, 0x5dc000);
    func_02012694(0x46, (int *)(thiz + 0x74));

    {
        int x, y, z;
        x = *(int *)(thiz + 0x5c);
        tmp.x = x;
        y = *(int *)(thiz + 0x60);
        tmp.y = y;
        z = *(int *)(thiz + 0x64);
        tmp.z = z;
        y = y - 0x78000;
        tmp.y = y;
        dust.x = x;
        dust.y = y;
        dust.z = z;
    }
    _ZN5Actor13LandingDustAtER7Vector3b(thiz, &dust, 1);

    *(int *)(thiz + 0x9c) = 0;
    return 0;
}
