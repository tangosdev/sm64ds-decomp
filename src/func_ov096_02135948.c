//cpp
typedef short s16;
typedef long long s64;

struct Vector3 { int x, y, z; };

extern "C" {
extern int _ZN5Actor7FindEggER12CylinderClsn(void* thiz, void* clsn);
extern int _ZN5Actor18FindExplosionActorER12CylinderClsn(void* thiz, void* clsn);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, void* pos);
extern void func_ov096_02135800(void* c);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* pos, const void* rot, int e, int f);
extern void func_ov096_0213585c(void* t);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int Vec3_HorzDist(const void* a, const void* b);
extern int Vec3_HorzAngle(const void* a, const void* b);
extern s16 data_02082214[];
extern unsigned _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned a, unsigned b, int f, int t1, int t2, const void* v, void* cb);
extern unsigned _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned a, unsigned b, int f, int t1, int t2, const void* v);
extern void func_ov096_02136928(void* c, int n);
extern int _ZN6Player9IsOnShellEv(void* p);
extern void _ZN6Player16IncMegaKillCountEv(void* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, void* v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);

void func_ov096_02135948(char* c)
{
    unsigned int id170;
    char* p;
    int r150;
    int angle;
    int a;
    s16 cs;
    int dist;
    s16 sn;
    int dist2;
    unsigned int flags;
    int st38c;
    char* q;
    struct Vector3 pos;
    int vv1[3];
    int vv2[3];
    int vv3[3];
    int vv4[3];
    int vv5[3];
    int x, y, z;

    if (_ZN5Actor7FindEggER12CylinderClsn(c, c + 0x14c) != 0 ||
        _ZN5Actor18FindExplosionActorER12CylinderClsn(c, c + 0x14c) != 0) {
        _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
        func_ov096_02135800(c);
        {
            int b = (*(unsigned short*)(c + 0xc) == 0xf0);
            if (b) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0x122, 2, c + 0x5c, 0, *(signed char*)(c + 0xcc), -1);
            }
        }
        func_ov096_0213585c(c);
        return;
    }

    id170 = *(unsigned int*)(c + 0x170);
    if (id170 == 0)
        return;

    p = (char*)_ZN5Actor10FindWithIDEj(id170);
    if (p == 0)
        return;

    if (*(int*)(c + 0x38c) == 1) {
        int b = (*(unsigned short*)(p + 0xc) == 0x135);
        if (b) {
            r150 = *(int*)(c + 0x150);
            if (Vec3_HorzDist(c + 0x5c, p + 0x5c) < r150 + 0x1a9000) {
                angle = Vec3_HorzAngle(p + 0x5c, c + 0x5c);
                a = ((unsigned short)angle >> 4) * 2;
                cs = data_02082214[a];
                dist = *(int*)(c + 0x150) + 0x1a9000;
                *(int*)(c + 0x5c) = *(int*)(p + 0x5c) + (int)(((s64)dist * cs + 0x800) >> 12);
                sn = data_02082214[a + 1];
                dist2 = *(int*)(c + 0x150) + 0x1a9000;
                *(int*)(c + 0x64) = *(int*)(p + 0x64) + (int)(((s64)dist2 * sn + 0x800) >> 12);
            }
        }
    }

    {
        int bf = (*(unsigned short*)(p + 0xc) == 0xbf);
        if (!bf)
            return;
    }

    flags = *(unsigned int*)(c + 0x16c);
    if (!(flags & 0x8000)) {
        if (flags & 0x40000) {
            x = *(int*)(c + 0x5c);
            z = *(int*)(c + 0x64);
            y = *(int*)(c + 0x60) + 0x3c000;
            ((int*)&pos)[0] = x;
            ((int*)&pos)[1] = y;
            ((int*)&pos)[2] = z;
            *(unsigned*)(c + 0x3a0) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(unsigned*)(c + 0x3a0), 0x13a, pos.x, pos.y, pos.z, 0, 0);
            *(unsigned*)(c + 0x3a4) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                *(unsigned*)(c + 0x3a4), 0x13b, pos.x, pos.y, pos.z, 0);
            _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
            *(void**)(c + 0x388) = p;
            func_ov096_02136928(c, 2);
        } else if ((flags & 0x26fe0) || _ZN6Player9IsOnShellEv(p) != 0 ||
                   *(unsigned char*)(p + 0x6f9) != 0) {
            _ZN5Sound9PlayBank0EjRK7Vector3(9, c + 0x74);
            *(void**)(c + 0x388) = p;
            func_ov096_02136928(c, 2);
        } else if (*(unsigned int*)(c + 0x16c) & 0x10) {
            *(void**)(c + 0x388) = p;
            _ZN6Player16IncMegaKillCountEv(p);
            func_ov096_02136928(c, 5);
        } else {
            st38c = *(int*)(c + 0x38c);
            if (st38c == 0) {
                q = *(char**)(c + 0x390);
                if (q == 0) {
                    if (*(int*)(c + 0xa8) == 0) {
                        vv1[0] = *(int*)(c + 0x5c);
                        vv1[1] = *(int*)(c + 0x60);
                        vv1[2] = *(int*)(c + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv1, 2, 0xc000, 1, 0, 1);
                    }
                } else if (*(int*)(c + 0xa8) == 0 && *(int*)(q + 0xa8) == 0) {
                    vv2[0] = *(int*)(c + 0x5c);
                    vv2[1] = *(int*)(c + 0x60);
                    vv2[2] = *(int*)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv2, 2, 0xc000, 1, 0, 1);
                }
            } else if (st38c == 1) {
                if (*(void**)(c + 0x390) == 0) {
                    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x180) != 0) {
                        vv3[0] = *(int*)(c + 0x5c);
                        vv3[1] = *(int*)(c + 0x60);
                        vv3[2] = *(int*)(c + 0x64);
                        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv3, 2, 0xc000, 1, 0, 1);
                    }
                } else if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x180) != 0 &&
                           *(int*)(*(char**)(c + 0x390) + 0xa8) == 0) {
                    vv4[0] = *(int*)(c + 0x5c);
                    vv4[1] = *(int*)(c + 0x60);
                    vv4[2] = *(int*)(c + 0x64);
                    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv4, 2, 0xc000, 1, 0, 1);
                }
            } else {
                vv5[0] = *(int*)(c + 0x5c);
                vv5[1] = *(int*)(c + 0x60);
                vv5[2] = *(int*)(c + 0x64);
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(p, vv5, 2, 0xc000, 1, 0, 1);
            }
        }
    }

    {
        int t = (*(unsigned int*)(c + 0xb0) & 0x20000) != 0;
        if (t) {
            func_ov096_02136928(c, 3);
        }
    }
}
}
