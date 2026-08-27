//cpp
// Matched byte-identical (mwccarm 1.2/sp2p3). The former div-2 "mov r2,#1 vs
// str ip,[sp,#8]" ordering residual was fixed by dropping volatile on tmp and
// making tmp/eq/dust members of ONE escaped stack struct: the escape keeps the
// dead tmp stores in memory while leaving the scheduler free to hoist the
// LandingDustAt bool-arg mov above the stores, exactly as the ROM has it.
struct Vector3 { int x, y, z; };
typedef short s16;
#define LA(p) (p)
extern "C" {
void *_ZN8dActor_c13ClosestPlayerEv(void *self);
int func_ov002_020de328(void);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *c);
void dBgCh_Actr_UpdateContinuous_Veneer(void *a);
int _ZNK10dBgCh_Actr10IsOnGroundEv(void *self);
void _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(void *self, const Vector3 *v, int fix);
void func_02012694(int a, int *t);
void _ZN8dActor_c13LandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
}
extern "C" int func_ov092_02131010(unsigned char *thiz)
{
    struct Frame { Vector3 tmp; Vector3 eq; Vector3 dust; } f;

    if (_ZN8dActor_c13ClosestPlayerEv(thiz) != 0) {
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

    _ZN8dActor_c9UpdatePosEP5dCc_c(thiz, 0);
    dBgCh_Actr_UpdateContinuous_Veneer(thiz + 0x324);
    {
        int g = _ZNK10dBgCh_Actr10IsOnGroundEv(thiz + 0x324);
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

    f.eq.x = *(int *)(thiz + 0x5c);
    f.eq.y = *(int *)(thiz + 0x60);
    f.eq.z = *(int *)(thiz + 0x64);
    _ZN8dActor_c10EarthquakeERK7Vector35Fix12IiE(thiz, &f.eq, 0x5dc000);
    func_02012694(0x46, (int *)(thiz + 0x74));

    {
        int x, y, z;
        x = *(int *)(thiz + 0x5c);
        f.tmp.x = x;
        y = *(int *)(thiz + 0x60);
        f.tmp.y = y;
        z = *(int *)(thiz + 0x64);
        f.tmp.z = z;
        y = y - 0x78000;
        f.tmp.y = y;
        f.dust.x = x;
        f.dust.y = y;
        f.dust.z = z;
    }
    _ZN8dActor_c13LandingDustAtER7Vector3b(thiz, &f.dust, 1);

    *(int *)(thiz + 0x9c) = 0;
    return 0;
}
