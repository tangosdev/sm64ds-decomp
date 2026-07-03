struct Vector3 { int x, y, z; };

extern int func_ov078_02123804(char* c);
extern void func_ov078_02123864(char* c);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov102_0214b384(void* a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* self, int frame);
extern void* _ZN5Actor13ClosestPlayerEv(void* actor);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void Matrix4x3_FromRotationY(void* m, int ang);
extern int Vec3_HorzLen(void* v);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern void func_ov102_0214ad14(void* c);
extern void MulVec3Mat4x3(void* a, void* m, void* b);
extern int _ZN9Animation8FinishedEv(void* self);
extern void func_ov078_02125c48(void* c, void* p);

extern int data_020a0e68[];
extern int data_ov078_0212703c;

int func_ov078_021250f8(char* c) {
    struct Vector3 in, out, v[2];
    void* target;
    void* player;

    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864(c);
        return 1;
    }

    if (*(unsigned char*)(c + *(int*)(c + 0x49c) + 0x42c) == 0) {
        target = _ZN5Actor10FindWithIDEj(*(int*)(c + (*(int*)(c + 0x49c) << 2) + 0x424));
        if (target != 0) {
            *(int*)((char*)target + 0x5c) = *(int*)(c + 0x4ec);
            *(int*)((char*)target + 0x60) = *(int*)(c + 0x4f0);
            *(int*)((char*)target + 0x64) = *(int*)(c + 0x4f4);
            *(short*)((char*)target + 0x92) = *(short*)(c + 0x92);
            *(short*)((char*)target + 0x94) = *(short*)(c + 0x94);
            *(short*)((char*)target + 0x96) = *(short*)(c + 0x96);
            *(short*)((char*)target + 0x8c) = *(short*)(c + 0x8c);
            *(short*)((char*)target + 0x8e) = *(short*)(c + 0x8e);
            *(short*)((char*)target + 0x90) = *(short*)(c + 0x90);
            func_ov102_0214b384(target, 0x78);
            if (_ZNK9Animation12WillHitFrameEi(c + 0x31c, 0x13) != 0
                || func_ov078_02123804(c) == 1) {
                in.x = 0; in.y = 0; in.z = 0x28000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = _ZN5Actor13ClosestPlayerEv(c);
                if (player != 0) {
                    int* q = (int*)(((int)player + 0x5c) & 0xFFFFFFFFFFFFFFFF);
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - *(int*)(c + 0x4ec);
                    v[0].y = v[1].y - *(int*)(c + 0x4f0);
                    v[0].z = v[1].z - *(int*)(c + 0x4f4);
                    Matrix4x3_FromRotationY(data_020a0e68,
                        _ZN4cstd5atan2E5Fix12IiES1_(v[0].x, v[0].z));
                    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68,
                        (short)(-_ZN4cstd5atan2E5Fix12IiES1_(v[0].y, Vec3_HorzLen(&v[0]))));
                    out.y += 0x14000;
                    func_ov102_0214ad14(target);
                    *(int*)((char*)target + 0xa4) = out.x;
                    *(int*)((char*)target + 0xa8) = out.y;
                    *(int*)((char*)target + 0xac) = out.z;
                    *(int*)((char*)target + 0x98) = 0x14000;
                    *(int*)((char*)target + 0x9c) = -0x2000;
                    MulVec3Mat4x3(&in, data_020a0e68, &out);
                }
                *(unsigned char*)(c + *(int*)(c + 0x49c) + 0x42c) = 1;
                *(int*)((char*)target + 0xc8) = 0;
            }
        }
    } else {
        if (func_ov078_02123804(c) == 1) {
            return 1;
        }
    }

    if (_ZN9Animation8FinishedEv(c + 0x31c) != 0) {
        player = _ZN5Actor13ClosestPlayerEv(c);
        if (player != 0) {
            if (*(int*)((char*)player + 8) != 3) {
                *(unsigned char*)(c + 0x504) = 0x64;
            }
        }
        func_ov078_02125c48(c, &data_ov078_0212703c);
    }
    return 1;
}
