//cpp
// NONMATCHING: size 0x17c vs 0x1a0 (delta -36); LandingDust double-store stack shape and angle pointer materialization residuals
struct Vector3 { int x, y, z; };
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
    Vector3 eq;
    Vector3 dust;
    Vector3 tmp;

    if (_ZN5Actor13ClosestPlayerEv(thiz) != 0) {
        if (func_ov002_020de328() != 0) {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)(thiz + 0x500);
            *p504 |= 0x400000;
            *p500 |= 4;
        } else {
            int *p504 = (int *)LA(thiz + 0x504);
            int *p500 = (int *)(thiz + 0x500);
            *p504 &= ~0x400000;
            *p500 &= ~4;
        }
    }

    {
        short *lr = (short *)(thiz + 0x8c);
        short *base = (short *)(thiz + 0x400);
        short *ip = (short *)(thiz + 0x8e);
        short *r2 = (short *)(thiz + 0x90);
        *lr = (short)(*lr + base[0xe0/2]);
        *ip = (short)(*ip + base[0xe2/2]);
        *r2 = (short)(*r2 + base[0xe4/2]);
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
        short *r3 = (short *)(thiz + 0x400);
        short lr = 0;
        r3[0xe4/2] = lr;
        r3[0xe2/2] = r3[0xe4/2];
        r3[0xe0/2] = r3[0xe2/2];
        *(int *)(thiz + 0x98) = lr;
        *(int *)(thiz + 0xa8) = lr;
    }

    eq.x = *(int *)(thiz + 0x5c);
    eq.y = *(int *)(thiz + 0x60);
    eq.z = *(int *)(thiz + 0x64);
    _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(thiz, &eq, 0x5dc000);
    func_02012694(0x46, (int *)(thiz + 0x74));

    tmp.x = *(int *)(thiz + 0x5c);
    tmp.y = *(int *)(thiz + 0x60);
    tmp.z = *(int *)(thiz + 0x64);
    tmp.y = tmp.y - 0x78000;
    dust.x = tmp.x;
    dust.y = tmp.y;
    dust.z = tmp.z;
    _ZN5Actor13LandingDustAtER7Vector3b(thiz, &dust, 1);

    *(int *)(thiz + 0x9c) = 0;
    return 0;
}
