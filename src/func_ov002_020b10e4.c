typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int Fix12i;

typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dActor_c dActor_c;

struct BF3ae {
    u8 b0 : 1;
    u8 : 4;
    u8 sel : 3;
};

extern signed char data_0209f2f8;

extern dActor_c* _ZN8dActor_c4NextEPKS_(const dActor_c* prev);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern void _ZN9dBgCh_LinC1Ev(void* self);
extern void _ZN9dBgCh_LinD1Ev(void* self);
extern void _ZN5dBgPiC1Ev(void* self);
extern void _ZN5dBgPiD1Ev(void* self);
extern int _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(void* self, const Vector3* a, const Vector3* b, dActor_c* obj);
extern int _ZN9dBgCh_Lin10DetectClsnEv(void* self);
extern void _ZNK5dBgPi6CopyToERS_(const void* self, void* dst);
extern u32 _ZNK5dBgPi9GetClsnIDEv(const void* self);
extern dActor_c* _ZN8dActor_c10FindWithIDEj(u32 id);
extern void _ZN9dBgCh_Lin10GetClsnPosEv(Vector3* res, void* self);

void func_ov002_020b10e4(char* c)
{
    int b;
    dActor_c* a;

    b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b) return;

    if (((struct BF3ae*)(c + 0x3ae))->sel != 7) return;

    if (data_0209f2f8 == 0x1c || data_0209f2f8 == 0x27) {
        a = _ZN8dActor_c4NextEPKS_(0);
        if (a) {
            do {
                char* ac = (char*)a;
                u16 type = *(u16*)(ac + 0xc);
                if (type == 0x7e || type == 0x81 || type == 0x9c) {
                    int dy = *(int*)(c + 0x60) - *(int*)(ac + 0x60);
                    int radius = *(int*)(ac + 0xb8);
                    int dist = Vec3_HorzDist((Vector3*)(c + 0x5c), (Vector3*)(ac + 0x5c));
                    if (dist < (radius << 3) && dy <= 0x1f4000 && dy >= 0) {
                        *(int*)(c + 0x398) = *(int*)(ac + 0x60) + 0x32000;
                        ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 1;
                        return;
                    }
                }
                a = _ZN8dActor_c4NextEPKS_(a);
            } while (a);
        }
    }

    {
        char rl[0x78];
        char cr[0x28];
        Vector3 va, vb;
        _ZN9dBgCh_LinC1Ev(rl);
        _ZN5dBgPiC1Ev(cr);
        vb.x = *(int*)(c + 0x5c);
        vb.y = *(int*)(c + 0x60);
        vb.z = *(int*)(c + 0x64);
        va.x = vb.x;
        va.y = vb.y;
        va.z = vb.z;
        va.y += 0x14000;
        vb.y -= 0x1f4000;
        _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(rl, &va, &vb, (dActor_c*)c);
        if (_ZN9dBgCh_Lin10DetectClsnEv(rl)) {
            _ZNK5dBgPi6CopyToERS_(rl + 0x10, cr);
            if (_ZNK5dBgPi9GetClsnIDEv(cr) != (u32)-1 &&
                _ZN8dActor_c10FindWithIDEj(_ZNK5dBgPi9GetClsnIDEv(cr)) != 0) {
                ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 0;
            } else {
                Vector3 pos;
                _ZN9dBgCh_Lin10GetClsnPosEv(&pos, rl);
                *(int*)(c + 0x398) = pos.y;
                ((struct BF3ae*)((long long)(c + 0x3ae)))->sel = 1;
            }
        }
        _ZN5dBgPiD1Ev(cr);
        _ZN9dBgCh_LinD1Ev(rl);
    }
}
