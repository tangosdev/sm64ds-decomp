typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int Fix12i;

typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct Actor Actor;

struct BF3ae {
    u8 b0 : 1;
    u8 : 4;
    u8 sel : 3;
};

extern signed char data_0209f2f8;

extern Actor* _ZN5Actor4NextEPKS_(const Actor* prev);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern void _ZN11RaycastLineC1Ev(void* self);
extern void _ZN11RaycastLineD1Ev(void* self);
extern void _ZN10ClsnResultC1Ev(void* self);
extern void _ZN10ClsnResultD1Ev(void* self);
extern int _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void* self, const Vector3* a, const Vector3* b, Actor* obj);
extern int _ZN11RaycastLine10DetectClsnEv(void* self);
extern void _ZNK10ClsnResult6CopyToERS_(const void* self, void* dst);
extern u32 _ZNK10ClsnResult9GetClsnIDEv(const void* self);
extern Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern void _ZN11RaycastLine10GetClsnPosEv(Vector3* res, void* self);

void func_ov002_020b10e4(char* c)
{
    int b;
    Actor* a;

    b = (int)((*(int*)(c + 0xb0) & 8) != 0);
    if (b) return;

    if (((struct BF3ae*)(c + 0x3ae))->sel != 7) return;

    if (data_0209f2f8 == 0x1c || data_0209f2f8 == 0x27) {
        a = _ZN5Actor4NextEPKS_(0);
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
                        ((struct BF3ae*)((long long)(c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL))->sel = 1;
                        return;
                    }
                }
                a = _ZN5Actor4NextEPKS_(a);
            } while (a);
        }
    }

    {
        char rl[0x78];
        char cr[0x28];
        Vector3 va, vb;
        _ZN11RaycastLineC1Ev(rl);
        _ZN10ClsnResultC1Ev(cr);
        vb.x = *(int*)(c + 0x5c);
        vb.y = *(int*)(c + 0x60);
        vb.z = *(int*)(c + 0x64);
        va.x = vb.x;
        va.y = vb.y;
        va.z = vb.z;
        va.y += 0x14000;
        vb.y -= 0x1f4000;
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(rl, &va, &vb, (Actor*)c);
        if (_ZN11RaycastLine10DetectClsnEv(rl)) {
            _ZNK10ClsnResult6CopyToERS_(rl + 0x10, cr);
            if (_ZNK10ClsnResult9GetClsnIDEv(cr) != (u32)-1 &&
                _ZN5Actor10FindWithIDEj(_ZNK10ClsnResult9GetClsnIDEv(cr)) != 0) {
                ((struct BF3ae*)((long long)(c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL))->sel = 0;
            } else {
                Vector3 pos;
                _ZN11RaycastLine10GetClsnPosEv(&pos, rl);
                *(int*)(c + 0x398) = pos.y;
                ((struct BF3ae*)((long long)(c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL))->sel = 1;
            }
        }
        _ZN10ClsnResultD1Ev(cr);
        _ZN11RaycastLineD1Ev(rl);
    }
}
