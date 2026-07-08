typedef short s16;
typedef struct { int x, y, z; } Vec3;

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(Vec3* in, void* m, Vec3* out);
extern void _Z15ApproachLinear2Rsss(s16* v, s16 target, s16 step);
extern void _Z14ApproachLinearRiii(int* v, int target, int step);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(Vec3* v, Vec3* target, int step);
extern int _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern void KillPlayer(void);

extern char MATRIX_SCRATCH_PAPER;

int func_ov026_02111b24(char* self)
{
    Vec3 v1;
    Vec3 v2;
    volatile Vec3 pv;
    char* player;

    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.x = 0;
    v2.y = 0;
    v2.z = 0;
    v1.z = *(int*)(self + 0x198);
    player = (char*)_ZN5Actor13ClosestPlayerEv(self);
    if (player) {
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(s16*)(self + 0x1b4));
        MulVec3Mat4x3(&v1, &MATRIX_SCRATCH_PAPER, &v2);
        v1.x = *(int*)(self + 0x1a8);
        v1.y = *(int*)(self + 0x1ac);
        v1.z = *(int*)(self + 0x1b0);
        _Z15ApproachLinear2Rsss((s16*)(self + 0x1b6), 0x1000, 0x200);
        _Z14ApproachLinearRiii((int*)(self + 0x198), 0x100, 0x1000);
        _Z14ApproachLinearRiii((int*)(self + 0x194), *(int*)(self + 0x1ac) - 0x64000, 0x4000);
        {
            s16* q = (s16*)(int)(((long long)(int)(self + 0x1b4)) & 0xFFFFFFFFFFFFFFFFLL);
            *q += *(s16*)(self + 0x1b6);
        }
        v1.y = *(int*)(self + 0x194);
        v1.x += v2.x;
        v1.z += v2.z;
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE((Vec3*)(self + 0x19c), &v1, 0x1e000);
        *(int*)(player + 0x5c) = *(int*)(self + 0x19c);
        *(int*)(player + 0x60) = *(int*)(self + 0x1a0);
        *(int*)(player + 0x64) = *(int*)(self + 0x1a4);
        {
            int ang = _ZN5Actor18HorzAngleToCPlayerEv(self) + 0x8000;
            *(s16*)(player + 0x8c) = -0x2000;
            *(s16*)(player + 0x8e) = ang;
            *(s16*)(player + 0x90) = 0;
        }
        {
            int* pp = (int*)(int)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            pv.x = pp[0];
            pv.y = pp[1];
            pv.z = pp[2];
            if (*(int*)(self + 0x1ac) > pp[1]) {
                if (*(int*)(self + 0x18c) == 0) {
                    KillPlayer();
                    *(int*)(self + 0x18c) = 1;
                }
            }
        }
    }
    return 1;
}
