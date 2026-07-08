//cpp
// NONMATCHING: different op / idiom (div=96). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;
typedef struct Vec3 { int x, y, z; } Vec3;

extern "C" {
void* func_ov002_020e496c(char* c);
int _ZN6Player14IsFrontSlidingEv(void* self);
int _ZN6Player17LostGrabbedObjectEv(void* self);
void Math_Function_0203b14c(void* out, int a1, int a2, int a3, int a4);
void* _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(void* self, void* player, const void* v);
void Matrix4x3_FromRotationY(void* m, int angle);
void MulMat4x3Mat4x3(void* a, void* b, void* out);
void Vec3_Lsl(Vec3* d, Vec3* s, int sh);
void Vec3_LslInPlace(Vec3* v, int sh);
int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    int sys, unsigned int kind, int fx, int a1, int a2, const void* v, void* cb);
void func_ov102_0214b444(void* c);

extern int data_ov102_0214ea18[];
extern int data_ov102_0214ea1c[];
extern int data_ov102_0214ea20[];
extern Mtx43 MATRIX_SCRATCH_PAPER;
extern s16 SINE_TABLE[];
}

extern "C" void func_ov102_0214b53c(char* c)
{
    char* src;

    if (*(u8*)(c + 0x3f5) == 2 && (src = *(char**)(c + 0xc8)) != 0) {
        *(Mtx43*)(c + 0x31c) = *(Mtx43*)src;
    } else {
        int haveMtx = (int)((*(u32*)(c + 0xb0) & 0x4000) != 0);
        char* player0;
        if (haveMtx != 0 && (player0 = *(char**)(c + 0x390)) != 0 && *(int*)(player0 + 0xc8) != 0) {
            char* ret = (char*)func_ov002_020e496c(player0);
            u8 idx = 0;
            if (_ZN6Player14IsFrontSlidingEv(*(char**)(c + 0x390)) != 0)
                idx = 1;
            if (_ZN6Player17LostGrabbedObjectEv(*(char**)(c + 0x390)) != 0
                && (u32)((u32)(*(int*)(ret + 0x58) << 4) >> 0x10) < 0xe)
                idx = 1;
            if (*(int*)(*(char**)(c + 0x390) + 8) == 2)
                idx = (u8)(idx + 2);

            int off = idx * 0xc;
            Math_Function_0203b14c(c + 0x3d0, *(int*)((char*)data_ov102_0214ea18 + off), 0x800, 0x3e8000, 4);
            Math_Function_0203b14c(c + 0x3d4, *(int*)((char*)data_ov102_0214ea1c + off), 0x800, 0x3e8000, 4);
            Math_Function_0203b14c(c + 0x3d8, *(int*)((char*)data_ov102_0214ea20 + off), 0x800, 0x3e8000, 4);

            Mtx43* rmtx = (Mtx43*)_ZN5Actor11UpdateCarryER6PlayerRK7Vector3(c, *(char**)(c + 0x390), c + 0x3d0);
            *(Mtx43*)(c + 0x31c) = *rmtx;
        } else {
            *(int*)(c + 0x3d0) = 0;
            *(int*)(c + 0x3d4) = 0;
            *(int*)(c + 0x3d8) = 0;
            Matrix4x3_FromRotationY(c + 0x31c, *(s16*)(c + 0x8e));
            *(int*)(c + 0x340) = *(int*)(c + 0x5c) >> 3;
            *(int*)(c + 0x344) = *(int*)(c + 0x60) >> 3;
            *(int*)(c + 0x348) = *(int*)(c + 0x64) >> 3;
        }
    }

    int haveMtx2 = (int)((*(u32*)(c + 0xb0) & 0x4000) != 0);
    if (haveMtx2 != 0 || *(u16*)(c + 0x3ea) != 0) {
        Vec3 p;
        *(volatile int*)&p.x = 0;
        *(volatile int*)&p.y = 0;
        *(volatile int*)&p.z = 0;

        MATRIX_SCRATCH_PAPER = *(Mtx43*)(c + 0x31c);
        MulMat4x3Mat4x3(*(char**)(c + 0x314) + 0x60, &MATRIX_SCRATCH_PAPER, &MATRIX_SCRATCH_PAPER);

        Vec3 s;
        s.x = MATRIX_SCRATCH_PAPER.w[9];
        s.y = MATRIX_SCRATCH_PAPER.w[10];
        s.z = MATRIX_SCRATCH_PAPER.w[11];
        Vec3 d;
        Vec3_Lsl(&d, &s, 3);

        p.x = d.x; p.y = d.y; p.z = d.z;

        int haveMtx3 = (int)((*(u32*)(c + 0xb0) & 0x4000) != 0);
        if (haveMtx3 != 0) {
            *(int*)(c + 0x3f8) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(int*)(c + 0x3f8), 0x13, p.x, p.y, p.z, 0, 0);
        }

        if (*(u16*)(c + 0x3ea) != 0) {
            int haveShadow = (int)((*(u32*)(c + 0xb0) & 0x40000) != 0);
            if (haveShadow != 0) {
                char* obj = *(char**)(c + 0xd0);
                if (obj != 0) {
                    volatile Vec3 dead;
                    dead.x = *(int*)(obj + 0x5c);
                    dead.y = *(int*)(obj + 0x60);
                    dead.z = *(int*)(obj + 0x64);

                    MATRIX_SCRATCH_PAPER = *(Mtx43*)(*(char**)(c + 0xd0) + 0x5ec);

                    p.x = MATRIX_SCRATCH_PAPER.w[9];
                    p.y = MATRIX_SCRATCH_PAPER.w[10];
                    p.z = MATRIX_SCRATCH_PAPER.w[11];
                    Vec3_LslInPlace(&p, 3);

                    char* p2 = *(char**)(c + 0xd0);
                    typedef struct { u16 ang8c, ang8e, ang90, v580, v582, v584; } Tmp;
                    volatile Tmp tmp;
                    tmp.ang8c = *(u16*)(p2 + 0x8c);
                    tmp.ang8e = *(u16*)(p2 + 0x8e);
                    tmp.ang90 = *(u16*)(p2 + 0x90);
                    tmp.v580 = *(u16*)(p2 + 0x580);
                    tmp.v582 = *(u16*)(p2 + 0x582);
                    tmp.v584 = *(u16*)(p2 + 0x584);

                    s16 diff = (s16)((s16)tmp.ang8e + 0x1800) - (s16)tmp.v582;
                    u16 ang8e = (u16)diff;
                    u16 v582 = tmp.v582;

                    p.y += 0x14000 + SINE_TABLE[(v582 >> 4) * 2] * 0x1e;

                    int idxX2 = ((u16)ang8e >> 4) * 2;
                    p.x += SINE_TABLE[idxX2] * 0x3c;
                    p.z += SINE_TABLE[idxX2 + 1] * 0x3c;
                }
            }
            *(int*)(c + 0x3fc) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                *(int*)(c + 0x3fc), 0x19, p.x, p.y, p.z, 0, 0);
        }
    }

    func_ov102_0214b444(c);
}
