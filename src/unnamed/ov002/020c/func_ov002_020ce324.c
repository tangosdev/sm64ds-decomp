typedef unsigned char u8;
typedef unsigned short u16;
typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dActor_c dActor_c;

struct S18  { short f0; char pad[0x16]; };
struct S18b { u16 f0; char pad[0x16]; };

extern u8 data_020a0e40;
extern struct S18b data_0209f49e[];
extern struct S18 data_0209f4a4[];
extern short data_02082214[];
extern char data_ov002_02110694;

extern int func_ov002_020ceb54(char* self);
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, const Vector3* a, const Vector3* b, dActor_c* obj);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(const void* self, Vector3* out);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern int AngleDiff(int a, int b);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* self, void* state);


int func_ov002_020ce324(char* c)
{
    Vector3 a1, a2, b1, b2, nrm;
    char rl1[0x78];
    int ok;

    if (!(data_0209f49e[data_020a0e40].f0 & 2)) return 0;
    if (func_ov002_020ceb54(c)) return 0;

    _ZN9dBgCh_LinC1Ev(rl1);
    a1.x = *(int*)(c + 0x5c);
    a1.y = *(int*)(c + 0x60);
    a1.z = *(int*)(c + 0x64);
    a2.x = *(int*)(c + 0x5c);
    a2.y = *(int*)(c + 0x60);
    a2.z = *(int*)(c + 0x64);
    a1.y += 0x50000;
    a2.y += 0x96000;
    _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl1, &a1, &a2, (dActor_c*)c);
    if (_ZN9dBgCh_Lin10DetectClsnEv(rl1)) {
        _ZN9dBgCh_LinD1Ev(rl1);
        return 0;
    }

    ok = 0;
    if (data_0209f4a4[data_020a0e40].f0 >= 0xa00) {
        ok = 1;
    } else {
        char rl2[0x78];
        int ax, bz, bx, az, by, ay;
        u16 ang = *(u16*)(c + 0x8e);

        _ZN9dBgCh_LinC1Ev(rl2);
        ax = *(int*)(c + 0x5c);
        az = *(int*)(c + 0x64);
        ay = *(int*)(c + 0x60) + 0x32000;
        b1.x = ax;
        b1.y = ay;
        b1.z = az;
        bz = *(int*)(c + 0x64) + data_02082214[(ang >> 4) * 2 + 1] * 0x64;
        bx = *(int*)(c + 0x5c) + data_02082214[(ang >> 4) * 2] * 0x64;
        by = *(int*)(c + 0x60) + 0x32000;
        b2.x = bx;
        b2.y = by;
        b2.z = bz;
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl2, &b1, &b2, (dActor_c*)c);
        if (_ZN9dBgCh_Lin10DetectClsnEv(rl2)) {
            _ZNK11SurfaceInfo12CopyNormalToER7Vector3(rl2 + 0x14, &nrm);
            if (nrm.y == 0) {
                if (AngleDiff(_ZN4cstd5atan2E5Fix12IiES1_(nrm.x, nrm.z), *(short*)(c + 0x8e)) >= 0x5000)
                    ok = 1;
            }
        }
        _ZN9dBgCh_LinD1Ev(rl2);
    }

    if (ok == 1) {
        *(u8*)(c + 0x706) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_02110694);
        _ZN9dBgCh_LinD1Ev(rl1);
        return 1;
    }
    _ZN9dBgCh_LinD1Ev(rl1);
    return 0;
}
