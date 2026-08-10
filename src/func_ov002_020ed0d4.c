// @symbol func_ov002_020ed0d4
// recovered name: daWarpkun_c_Kill
/* recovered: shared common types, renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method */
/* daWarpkun_c::Kill - recovered from vtable slot identity */

extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void MulVec3Mat4x3(void* a, void* m, void* out);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* self);
extern void Vec3_Add(void* out, void* a, void* b);
extern int Vec3_HorzDist(void* a, void* b);
extern short Vec3_HorzAngle(void* a, void* b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Math_Function_0203b14c(void* ptr, int target, int rate, int limit, int step);
extern void ApproachAngle(void* cur, short target, int divisor, int band, int maxStep);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* self, void* file, int a, int b, unsigned int c);
extern int func_ov002_020ec654(unsigned char* p);
extern void _ZN12CylinderClsn5ClearEv(void* self);
extern int Vec3_Dist(void* a, void* b);
extern void WithMeshClsn_UpdateDiscreteNoLava_veneer(void* self);

extern int data_020a0e68[];
extern void* data_ov002_0210e6b0[];
extern void* data_ov002_0210eb78[];

void func_ov002_020ed0d4(char* self)
{
    struct Vector3 in, out;
    char* com;
    int* p;
    volatile struct Vector3 t1, t2;
    struct Vector3 v1, v2;
    struct Vector3 sumA, sumB, sumC;
    int *pt1, *pt2, *pv1, *pv2;

    com = *(char**)(self + 0x38c);
    *(int*)(self + 0x3f4) = -0x68000;
    p = (int*)(com + 0xa4);
    *(int*)(self + 0xa4) = p[0];
    *(int*)(self + 0xa8) = p[1];
    *(int*)(self + 0xac) = p[2];
    *(int*)(self + 0x98) = *(int*)(com + 0x98);

    if (DecIfAbove0_Byte((unsigned char*)(self + 0x41e)) == 0) {
        in.x = 0; in.y = 0; in.z = 0;
        out.x = 0; out.y = 0; out.z = 0;
        in.z = *(int*)(self + 0x3f4);

        Matrix4x3_FromRotationZXYExt(data_020a0e68, *(short*)(self + 0x3ea), *(short*)(self + 0x3ec), *(short*)(self + 0x3ee));
        MulVec3Mat4x3(&in, data_020a0e68, &out);

        if (*(int*)(*(char**)(self + 0x38c) + 0x37c) != 0
            || func_ov002_020cf700(*(char**)(self + 0x38c)) != 0
            || func_ov002_020d0d2c(*(char**)(self + 0x38c)) != 0) {
            if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x144) != 0) {
                *(short*)(self + 0x3e4) = *(short*)(*(char**)(self + 0x38c) + 0x8c);
                *(short*)(self + 0x3e8) = *(short*)(*(char**)(self + 0x38c) + 0x90);
                pt1 = (int*)(com + 0x68);
                pt2 = (int*)(com + 0x5c);
                t1.x = pt1[0];
                t1.y = pt1[1];
                t1.z = pt1[2];
                t2.x = pt2[0];
                t2.y = pt2[1];
                t2.z = pt2[2];
                Vec3_Add(&sumA, com + 0x5c, &out);
                *(int*)(self + 0x3d8) = sumA.x;
                *(int*)(self + 0x3dc) = sumA.y;
                *(int*)(self + 0x3e0) = sumA.z;
            } else {
                *(short*)(self + 0x3e4) = -0x4000;
                *(short*)(self + 0x3e8) = 0;
                Vec3_Add(&sumB, com + 0x5c, &out);
                *(int*)(self + 0x3d8) = sumB.x;
                *(int*)(self + 0x3dc) = sumB.y;
                *(int*)(self + 0x3e0) = sumB.z;
            }
        } else {
            *(short*)(self + 0x3e4) = *(short*)(com + 0x8c);
            *(short*)(self + 0x3e8) = *(short*)(com + 0x90);
            pv1 = (int*)(com + 0x68);
            pv2 = (int*)(com + 0x5c);
            v1.x = pv1[0];
            v1.y = pv1[1];
            v1.z = pv1[2];
            v2.x = pv2[0];
            v2.y = pv2[1];
            v2.z = pv2[2];
            if (Vec3_HorzDist(&v1, &v2) >= 0x5000) {
                *(short*)(self + 0x3e6) = Vec3_HorzAngle(&v1, &v2);
            }
            Vec3_Add(&sumC, com + 0x5c, &out);
            *(int*)(self + 0x3d8) = sumC.x;
            *(int*)(self + 0x3dc) = sumC.y;
            *(int*)(self + 0x3e0) = sumC.z;
        }
        *(unsigned char*)(self + 0x41e) = 0;
    }

    {
        int fd = _ZN4cstd4fdivEii(0x1000, 0x3000);
        Math_Function_0203b14c(self + 0x5c, *(int*)(self + 0x3d8), fd, 0x3e8000, 4);
        Math_Function_0203b14c(self + 0x60, *(int*)(self + 0x3dc), fd, 0x3e8000, 4);
        Math_Function_0203b14c(self + 0x64, *(int*)(self + 0x3e0), fd, 0x3e8000, 4);
    }

    ApproachAngle(self + 0x3ea, *(short*)(self + 0x3e4), 8, 0x4000, 0x100);
    ApproachAngle(self + 0x3ec, *(short*)(self + 0x3e6), 8, 0x4000, 0x100);
    ApproachAngle(self + 0x3ee, *(short*)(self + 0x3e8), 8, 0x4000, 0x100);

    *(short*)(self + 0x8e) = *(short*)(self + 0x3ec);

    {
        int v98 = *(int*)(self + 0x98);
        int t = v98 >> 5;
        if (t > 0x1000) t = 0x1000;
        if (t < 0) t = 0;
        t = (t + 0x1000) >> 8;
        unsigned char byteVal = (unsigned char)t;
        int idx = byteVal & 0x3f;
        int speed = idx << 8;
        if (*(unsigned char*)(*(char**)(self + 0x38c) + 0x6de) != 0) speed = 0;
        if (v98 != 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov002_0210e6b0[1], 0, speed, 0);
        } else {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, data_ov002_0210eb78[1], 0, speed, 0);
        }
    }

    if (func_ov002_020ec654((unsigned char*)self) == 0) {
        int a4 = *(int*)(self + 0xa4);
        if (a4 < 0) a4 = -a4;
        *(int*)(self + 0xa4) = a4;
        int ac = *(int*)(self + 0xac);
        if (ac < 0) ac = -ac;
        *(int*)(self + 0xac) = ac;
        a4 = *(int*)(self + 0xa4);
        if (a4 > 0x20000) { a4 = 0x20000; *(int*)(self + 0xa4) = a4; }
        a4 = *(int*)(self + 0xa4);
        {
            int fd1 = _ZN4cstd4fdivEii(a4, 0x20000);
            int r1 = (int)(((long long)fd1 * 0xe39 + 0x800) >> 12);
            short a4ang = (short)(-r1);
            ac = *(int*)(self + 0xac);
            if (ac > 0x20000) { ac = 0x20000; *(int*)(self + 0xac) = ac; }
            ac = *(int*)(self + 0xac);
            {
                int fd2 = _ZN4cstd4fdivEii(ac, 0x20000);
                int r2 = (int)(((long long)fd2 * 0xe39 + 0x800) >> 12);
                *(short*)(self + 0x8c) = a4ang;
                *(short*)(self + 0x90) = (short)r2;
            }
        }
    } else {
        func_ov002_020ed738(self);
    }

    _ZN12CylinderClsn5ClearEv(self + 0x110);
    {
        int dist = Vec3_Dist(self + 0x5c, com + 0x5c);
        if (*(unsigned char*)(*(char**)(self + 0x38c) + 0x709) != 0) return;
        if (dist >= 0x190000) return;
        WithMeshClsn_UpdateDiscreteNoLava_veneer(self + 0x144);
    }
}
