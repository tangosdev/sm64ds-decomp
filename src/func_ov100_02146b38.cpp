//cpp
struct C;
typedef void (C::*PMF)();
struct Entry { PMF pmf; };
extern "C" Entry data_ov100_02148a1c[];
struct Vec3 { int x, y, z; };
struct M48 { int w[12]; };
extern "C" M48 MATRIX_SCRATCH_PAPER;

extern "C" int* _ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" int func_ov100_0214639c(int* p);
extern "C" void _ZN9ActorBase18MarkForDestructionEv(C* c);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(C* c, int clsn);
extern "C" void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(M48* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(M48* m, short angY);
extern "C" void _ZN9Animation7AdvanceEv(void* anim);

struct C {
    char pad[0x5c];
    Vec3 pos;        // 0x5c
    char pad2[0x8e - 0x68];
    short f8e;       // 0x8e
    char pad3[0x94 - 0x90];
    short f94;       // 0x94
    char pad4[0xf0 - 0x96];
    M48 mtx;         // 0xf0
    char pad5[0x124 - 0x120];
    int anim;        // 0x124
    char pad6[0x13c - 0x128];
    unsigned int id; // 0x13c
    char pad7[0x14c - 0x140];
    int idx;         // 0x14c
    int counter;     // 0x150
    char pad9[0x159 - 0x154];
    unsigned char flag; // 0x159
};

extern "C" int func_ov100_02146b38(C* c)
{
    Vec3 v;
    int* r;
    if (c->flag != 0) {
        (c->*data_ov100_02148a1c[c->idx].pmf)();
    } else {
        r = _ZN5Actor10FindWithIDEj(c->id);
        if (r == 0 || func_ov100_0214639c(r) != 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
        } else {
            (c->*data_ov100_02148a1c[c->idx].pmf)();
            _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
            {
                int* cp = (int*)(((int)c + 0x150) & 0xFFFFFFFFFFFFFFFF);
                *cp = *cp + 1;
            }
        }
        Vec3_Asr(&v, &c->pos, 3);
        Matrix4x3_FromTranslation(&MATRIX_SCRATCH_PAPER, v.x, v.y, v.z);
        c->f8e = c->f94;
        Matrix4x3_ApplyInPlaceToRotationY(&MATRIX_SCRATCH_PAPER, c->f8e);
        c->mtx = MATRIX_SCRATCH_PAPER;
        _ZN9Animation7AdvanceEv(&c->anim);
    }
    return 1;
}
