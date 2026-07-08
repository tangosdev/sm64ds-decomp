// NONMATCHING: different op / idiom (div=104). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
extern void func_ov078_02125c24(char* c, int a);
extern int _ZN5Actor18HorzAngleToCPlayerEv(char* c);
extern void ApproachAngle(short* p, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* pl, char* c, int b);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int f);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* pl, char* c, int m, void* v, int a, int b);
extern void func_02012694(int a, void* b);
extern void func_ov078_02125c48(char* c, void* pmf);
extern void _ZN5Actor15HugeLandingDustEb(char* c, int b);
extern int Vec3_Dist(const void* a, const void* b);
extern int RandomIntInternal(int* seed);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player12Unk_020c6a10Ej(void* p, unsigned int a);

extern unsigned char STAR_ID;
extern char data_ov078_0212705c;
extern char data_ov078_0212702c;
extern int RNG_STATE;

struct V3 { int x, y, z; };

int func_ov078_021240a0(char* c)
{
    struct V3 v;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x110)) {
        if (*(int*)(c + 0x500) <= 0) {
            if (*(int*)(c + 0x98) != 0) {
                *(int*)(c + 0x98) = 0;
                func_ov078_02125c24(c, 0x7d0000);
            }
            if ((*(unsigned short*)(*(int*)(c + 0x430) + 0x6ce) & 0x800) != 0)
                return 1;
            _ZN5Actor18HorzAngleToCPlayerEv(c);
            ApproachAngle((short*)(c + 0x94), _ZN5Actor18HorzAngleToCPlayerEv(c), 5, 0x1000, 0x200);
            *(short*)(c + 0x8e) = *(short*)(c + 0x94);
            if (AngleDiff(_ZN5Actor18HorzAngleToCPlayerEv(c), *(short*)(c + 0x8e)) < 0x1000) {
                char* pl = (char*)(long)*(int*)(c + 0x430);
                if (_ZN6Player9StartTalkER9ActorBaseb(pl, c, 1)) {
                    int msg;
                    if (STAR_ID == 1) {
                        msg = (short)(*(int*)(pl + 8) + 0x9a);
                    } else {
                        msg = 0x95;
                    }
                    _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x14, 0x15666);
                    if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(pl, c, msg, c + 0x5c, 0, 0)) {
                        func_02012694(0x12a, c + 0x74);
                        func_ov078_02125c48(c, &data_ov078_0212705c);
                    }
                }
            }
        } else {
            if (*(unsigned char*)(c + 0x499) == 0) {
                func_02012694(0x128, c + 0x74);
                func_ov078_02125c24(c, 0x7d0000);
                _ZN5Actor15HugeLandingDustEb(c, 1);
                *(unsigned char*)(c + 0x499) = 1;
            }
            if (*(unsigned short*)(c + 0x100) == 0) {
                *(int*)(c + 0x49c) = 0;
                func_ov078_02125c48(c, &data_ov078_0212702c);
                return 1;
            }
            if (Vec3_Dist(c + 0x4d4, c + 0x5c) < 0x258000) {
                int r = RandomIntInternal(&RNG_STATE);
                int d = ((r >> 8) & 0xf) << 0x1c >> 0x10;
                if (d < -0x4000) d = -0x4000;
                else if (d > 0x4000) d = 0x4000;
                *(short*)(c + 0x4fa) = *(short*)(c + 0x4fa) + d;
            } else {
                *(short*)(c + 0x4fa) = Vec3_HorzAngle(c + 0x5c, c + 0x4d4);
            }
            *(unsigned char*)(c + 0x499) = 0;
            *(int*)(c + 0xa8) = 0x1e000;
            *(int*)(c + 0x98) = 0xa000;
        }
    } else {
        if (*(int*)(c + 0xa8) < 0 && *(int*)(c + 0x360) != 0) {
            char* a = (char*)_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x360));
            if (a != 0) {
                int b = (*(unsigned short*)(a + 0xc) == 0xbf);
                if (b != 0) {
                    v = *(struct V3*)(a + 0x5c);
                    if (*(int*)(c + 0x60) > v.y)
                        _ZN6Player12Unk_020c6a10Ej(a, 1);
                }
            }
        }
    }

    *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    ApproachAngle((short*)(c + 0x94), *(short*)(c + 0x4fa), 5, 0x1000, 0x200);
    return 1;
}
