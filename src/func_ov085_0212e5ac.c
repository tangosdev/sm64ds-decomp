typedef struct Vector3 { int x, y, z; } Vector3;

extern void* CAMERA;
extern signed char LEVEL_ID;
extern int MATRIX_SCRATCH_PAPER[];

extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void _ZN5Actor13SpawnSoundObjEj(void* self, unsigned int a);
extern int Vec3_HorzDist(Vector3* a, Vector3* b);
extern short Vec3_HorzAngle(Vector3* a, Vector3* b);
extern void Matrix4x3_FromRotationY(void* m, int ang);
extern void MulVec3Mat4x3(Vector3* a, void* m, Vector3* b);

int func_ov085_0212e5ac(char* self)
{
    Vector3 in, out, plpos;
    char* pl;
    Vector3* src;
    void* p;

    pl = CAMERA;
    in.x = 0; in.y = 0; in.z = 0;
    out.x = 0; out.y = 0; out.z = 0;
    src = (Vector3*)(((int)pl + 0x8c) & 0xFFFFFFFFFFFFFFFF);
    plpos.x = src->x;
    plpos.y = src->y;
    plpos.z = src->z;
    if (LEVEL_ID == 0x2f) {
        *(int*)(self + 0x5c) = 0;
    } else {
        *(int*)(self + 0x5c) = 0x1086000;
        if (*(int*)(self + 0x2cc) == 0) {
            p = _ZN5Actor13ClosestPlayerEv(self);
            if (p != 0 && *(int*)((char*)p + 0x5c) > 0x1086000) {
                _ZN5Actor13SpawnSoundObjEj(self, 0);
                *(int*)(self + 0x2cc) = 1;
            }
        }
    }
    *(int*)(self + 0x60) = plpos.y;
    *(int*)(self + 0x64) = plpos.z;
    in.z = Vec3_HorzDist(&plpos, (Vector3*)(self + 0x5c));
    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, Vec3_HorzAngle(&plpos, (Vector3*)(self + 0x5c)));
    MulVec3Mat4x3(&in, MATRIX_SCRATCH_PAPER, &out);
    *(int*)(((int)self + 0x5c) & 0xFFFFFFFFFFFFFFFF) += out.x;
    if (LEVEL_ID == 0x2f) {
        if (*(int*)(self + 0x5c) > 0x79e000) {
            *(int*)(self + 0x5c) = 0x79e000;
        }
    }
    *(int*)(((int)self + 0x64) & 0xFFFFFFFFFFFFFFFF) += out.z;
    *(short*)(self + 0x94) = 0x8000 - *(short*)(pl + 0x17c);
    *(short*)(self + 0x92) = -*(short*)(pl + 0x17e);
    return 1;
}
