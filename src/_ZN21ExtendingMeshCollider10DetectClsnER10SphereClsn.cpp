//cpp
typedef long long s64;
typedef unsigned char u8;

typedef struct {
    int head[4];         /* 0x00 */
    char result[0x60];   /* 0x10 */
    u8 flags;            /* 0x70 */
    char pad71[3];
    char floorRes[0x28]; /* 0x74 */
    char wallRes[0x28];  /* 0x9c */
    char undRes[0x28];   /* 0xc4 */
    int f_ec;            /* 0xec */
    int f_f0[3];         /* 0xf0 */
    int f_fc;             /* 0xfc */
    int f_100;             /* 0x100 */
    int tail[3];           /* 0x104..0x110 */
} LocSphere;

extern "C" {
void func_0203abb0(int* a, int* b);
void func_0203aa74(void* thiz, int* v, int* res);
void _ZN10SphereClsnC1Ev(void* o);
void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void* o, void* v, int r, void* a);
void func_02037940(void* p, int v);
void func_02035394(void* o, void* r);
int _ZN12MeshCollider10DetectClsnER10SphereClsn(void* o, void* s);
void func_02037a04(void* o, void* d1, void* d2);
void func_02037a6c(void* b, int x1, int y1, int z1, int x2, int y2, int z2);
void _ZN10ClsnResultaSERKS_(void* d, void* s);
void* func_02037938(void* p);
void _ZN10SphereClsn14SetFloorResultERK10ClsnResult(void* o, void* r);
void func_0203794c(void* d, void* s);
void* func_020378dc(void* p);
void func_02037888(void* d, void* s);
void* func_02037880(void* p);
void func_0203782c(void* d, void* s);
void _ZN10SphereClsnD1Ev(void* o);
}

#pragma opt_common_subs off

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

extern "C" int _ZN21ExtendingMeshCollider10DetectClsnER10SphereClsn(char* self, char* sphere)
{
    int v1[3];
    int v2[3];
    int d[12];
    LocSphere loc;
    int scale;
    int radius1;
    int radius2;
    int r;

    func_0203abb0((int*)(sphere + 0x38), v1);
    func_0203aa74(self, v1, v2);

    scale = *(int*)(self + 0x164);
    radius1 = FMUL(*(int*)(sphere + 0x48), scale);
    radius2 = FMUL(*(int*)(sphere + 0xec), scale);

    _ZN10SphereClsnC1Ev(&loc);
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(&loc, v2, radius1, 0);
    loc.f_ec = radius2;
    func_02037940(&loc, *(u8*)(sphere + 0x70));
    func_02035394(&loc, sphere);
    r = _ZN12MeshCollider10DetectClsnER10SphereClsn(self, &loc);
    if (r) {
        func_02037a04(&loc, d, d + 3);
        d[6] = FMUL(d[0], *(int*)(self + 0x50));
        d[7] = FMUL(d[1], *(int*)(self + 0x50));
        d[8] = FMUL(d[2], *(int*)(self + 0x50));
        d[9] = FMUL(d[3], *(int*)(self + 0x50));
        d[10] = FMUL(d[4], *(int*)(self + 0x50));
        d[11] = FMUL(d[5], *(int*)(self + 0x50));
        func_02037a6c(sphere, d[6], d[7], d[8], d[9], d[10], d[11]);
        _ZN10ClsnResultaSERKS_(sphere + 0x10, loc.result);
        *(u8*)(((s64)(int)(sphere + 0x70)) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
        if (loc.flags & 4) {
            if (*(u8*)(sphere + 0x70) & 4) {
                r &= ~1;
            } else {
                _ZN10SphereClsn14SetFloorResultERK10ClsnResult(sphere, func_02037938(&loc));
            }
            *(u8*)(((s64)(int)(sphere + 0x70)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
            if (*(int*)(sphere + 0x100) < loc.f_100) {
                func_0203794c(sphere, &loc.f_fc);
            }
        }
        if (loc.flags & 8) {
            func_02037888(sphere, func_020378dc(&loc));
            *(u8*)(((s64)(int)(sphere + 0x70)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
        }
        if (loc.flags & 0x10) {
            func_0203782c(sphere, func_02037880(&loc));
            *(u8*)(((s64)(int)(sphere + 0x70)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10;
        }
    }
    _ZN10SphereClsnD1Ev(&loc);
    return r;
}
