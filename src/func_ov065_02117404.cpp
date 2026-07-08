//cpp
struct Vector3 { int x, y, z; };

extern "C" {
int Vec3_Dist(const Vector3* a, const Vector3* b);
int WithMeshClsn_IsOnWall(void* self);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
void func_ov065_02117944(unsigned char* c, void* p);
void Matrix4x3_FromRotationY(void* m, int angle);
void ApproachAngle(short* a, int b, int c, int d, int e);
void MulVec3Mat4x3(const Vector3* in, const void* m, Vector3* out);

extern int data_ov065_0211d700;
extern int data_ov065_0211d6f0;
extern int MATRIX_SCRATCH_PAPER;
}

extern "C" int func_ov065_02117404(unsigned char* thiz)
{
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    int dist = Vec3_Dist((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x424));
    if (dist <= 0x1f4000) {
        if (WithMeshClsn_IsOnWall(thiz + 0x144) == 0) goto skip;
    }
    *(short*)(thiz + 0x434) = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), (Vector3*)(thiz + 0x424));
    if (*(unsigned short*)(thiz + 0x100) < 0x14) *(unsigned short*)(thiz + 0x100) = 0x14;
skip:
    if (dist < 0x1f4000 && (*(int*)(thiz + 0xb0) & 8) != 0) {
        int* ip1 = (int*)(((int)thiz + 0x430) & 0xFFFFFFFFFFFFFFFFULL);
        *ip1 = *ip1 + 1;
        if (*(int*)(thiz + 0x430) > 0x64) {
            *(int*)(thiz + 0x430) = 0;
            int* ip2 = (int*)(((int)thiz + 0xb0) & 0xFFFFFFFFFFFFFFFFULL);
            *ip2 = *ip2 | 1;
            *(short*)(thiz + 0x92) = (short)-0x8000;
            *(short*)(thiz + 0x8c) = *(short*)(thiz + 0x92);
            *(int*)(thiz + 0x5c) = *(int*)(thiz + 0x424);
            *(int*)(thiz + 0x60) = *(int*)(thiz + 0x428);
            *(int*)(thiz + 0x64) = *(int*)(thiz + 0x42c);
            *(int*)(thiz + 0xa4) = 0;
            *(int*)(thiz + 0xa8) = 0;
            *(int*)(thiz + 0xac) = 0;
            func_ov065_02117944(thiz, &data_ov065_0211d700);
            return 1;
        }
    }

    ApproachAngle((short*)(thiz + 0x92), 0, 0xa, 0x200, 0x100);
    ApproachAngle((short*)(thiz + 0x94), *(short*)(thiz + 0x434), 0xa, 0x200, 0x100);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, *(short*)(thiz + 0x8e));
    MulVec3Mat4x3(&v, &MATRIX_SCRATCH_PAPER, (Vector3*)(thiz + 0xa4));
    if (*(unsigned short*)(thiz + 0x100) == 0) {
        func_ov065_02117944(thiz, &data_ov065_0211d6f0);
    }
    return 1;
}
