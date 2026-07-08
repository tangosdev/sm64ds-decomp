//cpp
struct Vector3 { int x, y, z; };

extern "C" void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" int _ZNK9Animation12WillHitFrameEi(void* self, int frame);
extern "C" void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern "C" int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern "C" void Matrix4x3_FromRotationY(void* m, int ang);
extern "C" int Vec3_HorzLen(void* v);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern "C" void MulVec3Mat4x3(void* a, void* m, void* b);
extern "C" int _ZN9Animation8FinishedEv(void* self);
extern "C" void func_ov081_02125488(void* c, void* p);

extern "C" int MATRIX_SCRATCH_PAPER[];
extern "C" void* data_ov081_02128e14;

extern "C" int func_ov081_02124b98(char* c) {
    Vector3 in, out, v[2];
    void* target;
    void* player;

    if (*(unsigned int*)(c + 0x3fc) != 0) {
        target = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x3fc));
        if (target != 0) {
            *(int*)((char*)target + 0x5c) = *(int*)(c + 0x434);
            *(int*)((char*)target + 0x60) = *(int*)(c + 0x438);
            *(int*)((char*)target + 0x64) = *(int*)(c + 0x43c);
            if (_ZNK9Animation12WillHitFrameEi(c + 0x35c, 0xa) != 0) {
                in.x = 0; in.y = 0; in.z = 0x1e000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = _ZN5Actor13ClosestPlayerEv(c);
                if (player != 0) {
                    int angle;
                    int* q = (int*)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - *(int*)(c + 0x434);
                    v[0].y = v[1].y - *(int*)(c + 0x438);
                    v[0].z = v[1].z - *(int*)(c + 0x43c);
                    angle = _ZN4cstd5atan2E5Fix12IiES1_(v[0].x, v[0].z);
                    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, angle);
                    Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER,
                        (short)(-_ZN4cstd5atan2E5Fix12IiES1_(v[0].y, Vec3_HorzLen(&v[0]))));
                    MulVec3Mat4x3(&in, MATRIX_SCRATCH_PAPER, &out);
                }
                out.y += 0x14000;
                *(int*)((char*)target + 0xa4) = out.x;
                *(int*)((char*)target + 0xa8) = out.y;
                *(int*)((char*)target + 0xac) = out.z;
                *(int*)((char*)target + 0x9c) = -0x2000;
                *(unsigned int*)(c + 0x3fc) = 0;
            }
        }
    }
    if (_ZN9Animation8FinishedEv(c + 0x35c) != 0) {
        func_ov081_02125488(c, &data_ov081_02128e14);
    }
    return 1;
}
