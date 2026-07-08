// NONMATCHING: extra logic (you do more) (div=22). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;

struct Vec3 { int x, y, z; };
struct M12 { int w[12]; };

extern int _ZN6Player12GetTalkStateEv(void *p);
extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Vec3_Lsl(struct Vec3 *out, struct Vec3 *in, int sh);
extern void func_02012694(int a, void *p);
extern void _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(void *c, signed char *s, unsigned int n, struct Vec3 *pos, unsigned int u);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Actor16TriplePoofDustAtERK7Vector3(void *c, struct Vec3 *pos);
extern void _ZN5Sound22StopLoadedMusic_Layer3Ev(void);
extern void func_02011cfc(void);
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(unsigned int a, int b);
extern void _ZN9ActorBase18MarkForDestructionEv(void *c);
extern void *CAMERA;
extern int MATRIX_SCRATCH_PAPER[];

int func_ov078_021238ac(char *c)
{
    void *cam = CAMERA;
    struct Vec3 v;
    struct Vec3 t;
    struct Vec3 u;

    if (_ZN6Player12GetTalkStateEv(*(void**)(c + 0x430)) != -1) {
        _ZN6Camera9SetFlag_3Ev(cam);
        return 1;
    }

    {
        int *f = (int*)((char*)cam + 0x154);
        *f &= ~8;
    }
    v.x = 0;
    v.y = 0;
    v.z = 0;
    *(struct M12*)MATRIX_SCRATCH_PAPER = *(struct M12*)(c + 0x2e8);
    MulMat4x3Mat4x3((char*)*(void**)(c + 0x2e0) + 0x30, MATRIX_SCRATCH_PAPER, MATRIX_SCRATCH_PAPER);

    v.x = MATRIX_SCRATCH_PAPER[0x24 / 4];
    v.y = MATRIX_SCRATCH_PAPER[0x28 / 4];
    v.z = MATRIX_SCRATCH_PAPER[0x2c / 4];
    Vec3_Lsl(&t, &v, 3);
    v.x = t.x;
    v.y = t.y;
    v.z = t.z;
    func_02012694(0x130, c + 0x74);

    _ZN5Actor19UntrackAndSpawnStarERajRK7Vector3j(
        c, (signed char*)(c + 0x507), *(u8*)(c + 0x509), &v, 4);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1a, v.x, v.y, v.z);
    u.x = v.x;
    u.y = v.y;
    u.z = v.z;
    _ZN5Actor16TriplePoofDustAtERK7Vector3(c, &u);

    if (*(u8*)(c + 0x506) == 1) {
        *(u8*)(c + 0x506) = 0;
        _ZN5Sound22StopLoadedMusic_Layer3Ev();
        func_02011cfc();
        _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x7f, 0x15666);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    return 1;
}
