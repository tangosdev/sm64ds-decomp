//cpp
typedef int s32;
typedef short s16;

struct Vector3 { s32 x, y, z; };
struct Matrix4x3 { s32 m[12]; };

extern "C" void InvMat4x3(struct Matrix4x3* dst, struct Matrix4x3* src);
extern "C" void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3* m, s32 x, s32 y, s32 z);
extern "C" void func_02039e18(void* m, void* v, void* c);
extern "C" int LenVec3(struct Vector3* v);
extern "C" void func_01ffb0a4(void* self);
extern "C" void NormalizeVec3(struct Vector3* src, struct Vector3* dst);
extern "C" int func_01ffb07c(void* self, void* v);
extern "C" void func_020396d0(int* p, int v);
extern "C" void func_01ffb0b0(void* self);
extern "C" void func_01ffb0bc(void* self);

extern struct Matrix4x3 data_020a0e68;

struct VObj {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12(struct Vector3* v);
};

extern "C" void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(char* self, struct Matrix4x3* mtx, s16 a)
{
    *(struct Matrix4x3*)(self + 0xe4) = *(struct Matrix4x3*)(self + 0x54);
    InvMat4x3((struct Matrix4x3*)(self + 0xe4), (struct Matrix4x3*)(self + 0xe4));

    int zero = 0;
    *(struct Matrix4x3*)(self + 0x54) = *mtx;

    s16 oldAngle = *(s16*)(self + 0x114);
    *(s16*)(self + 0x116) = a - oldAngle;
    *(s16*)(self + 0x114) = a;

    *(s32*)(self + 0x124) = mtx->m[9] - *(s32*)(self + 0x118);
    *(s32*)(self + 0x128) = mtx->m[10] - *(s32*)(self + 0x11c);
    *(s32*)(self + 0x12c) = mtx->m[11] - *(s32*)(self + 0x120);
    *(s32*)(self + 0x118) = mtx->m[9];
    *(s32*)(self + 0x11c) = mtx->m[10];
    *(s32*)(self + 0x120) = mtx->m[11];

    struct Matrix4x3 mat = *(struct Matrix4x3*)(self + 0x54);
    mat.m[9] = zero;
    mat.m[10] = zero;
    mat.m[11] = zero;
    *(struct Matrix4x3*)(self + 0x84) = mat;
    InvMat4x3((struct Matrix4x3*)(self + 0x84), (struct Matrix4x3*)(self + 0x84));

    *(struct Matrix4x3*)(self + 0xb4) = *(struct Matrix4x3*)(self + 0x134);
    data_020a0e68 = *mtx;
    s32 scale = *(s32*)(self + 0x50);
    Matrix4x3_ApplyInPlaceToScale(&data_020a0e68, scale, scale, scale);
    *(struct Matrix4x3*)(self + 0x134) = data_020a0e68;
    *(struct Matrix4x3*)(self + 0x198) = *(struct Matrix4x3*)(self + 0x168);
    *(struct Matrix4x3*)(self + 0x168) = *(struct Matrix4x3*)(self + 0x134);
    InvMat4x3((struct Matrix4x3*)(self + 0x168), (struct Matrix4x3*)(self + 0x168));

    struct Vector3 up;
    up.x = 0; up.y = 0x1000; up.z = 0;
    func_02039e18(self, &up, self + 0x28);

    if (*(unsigned char*)(self + 0x130) == 0) return;

    struct Vector3 v;
    ((struct VObj*)self)->v12(&v);
    v.y = 0;
    if (LenVec3(&v) != 0) {
        func_01ffb0a4(self);
        NormalizeVec3(&v, &v);
        struct Vector3 v2;
        func_02039e18(self, &v, &v2);
        func_01ffb07c(self, &v2);
        func_020396d0((int*)self, 0xe66);
        func_01ffb0b0(self);
    } else {
        func_01ffb0bc(self);
    }
}
