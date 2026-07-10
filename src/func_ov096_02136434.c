//cpp
extern "C" {
struct Vector3 { int x, y, z; };
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *c, void *clsn);
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, int f, int t1, int t2, const void *v, void *cb);
extern unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, int f, int t1, int t2, const void *v);
extern void *_ZN8Particle6System12FromUniqueIDEj(unsigned id);
extern void func_02038414(void *p);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *p);
extern void func_ov096_0213585c(void *t);

int func_ov096_02136434(void *c)
{
    char *s = (char*)c;
    struct Vector3 pos;
    void *p0, *p1;
    int x, y, z;
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, s+0x14c);
    if (*(unsigned*)(s+0x3a0) != 0 && *(unsigned*)(s+0x3a4) != 0) {
        x = *(int*)(s+0x5c);
        z = *(int*)(s+0x64);
        y = *(int*)(s+0x60) + 0x3c000;
        ((int*)&pos)[0] = x;
        ((int*)&pos)[1] = y;
        ((int*)&pos)[2] = z;
        *(unsigned*)(s+0x3a0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(unsigned*)(s+0x3a0), 0x13a, pos.x, pos.y, pos.z, 0, 0);
        *(unsigned*)(s+0x3a4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(unsigned*)(s+0x3a4), 0x13b, pos.x, pos.y, pos.z, 0);
        p0 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned*)(s+0x3a0));
        p1 = _ZN8Particle6System12FromUniqueIDEj(*(unsigned*)(s+0x3a4));
        if (p0) *(int*)((char*)p0+0x50) = 0x7fff;
        if (p1) *(int*)((char*)p1+0x50) = 0x7fff;
    }
    func_02038414(s+0x180);
    if (_ZNK12WithMeshClsn13JustHitGroundEv(s+0x180) != 0)
        func_ov096_0213585c(c);
    return 1;
}
}
