//cpp
typedef unsigned int u32;
typedef short s16;
typedef int Fix12i;

struct Vec3 { Fix12i x, y, z; };
struct Mtx43 { Fix12i a[12]; };

extern "C" void Vec3_Asr(struct Vec3* d, struct Vec3* s, int sh);
extern "C" void Matrix4x3_FromTranslation(struct Mtx43* m, Fix12i x, Fix12i y, Fix12i z);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationZXYExt(void* m, int x, int y, int z);
extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* sm, void* mtx, Fix12i fx, int t, u32 u);

extern struct Mtx43 data_020a0e68;

struct VObj {
    virtual void v00(); virtual void v01(); virtual void v02(); virtual void v03();
    virtual void v04(); virtual void v05(); virtual void v06(); virtual void v07();
    virtual void v08(); virtual void v09(); virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual int  m29();  // slot 0x74 = index 29
};

enum Bool { FALSE, TRUE };

extern "C" void func_ov096_02135efc(char* c)
{
    struct Vec3 v;
    enum Bool b;

    if (*(int*)(c + 0x38c) == 5) {
        b = (enum Bool)(*(unsigned short*)(c + 0xc) == 0xf0);
        if (b) {
            int y1, y2;

            Vec3_Asr(&v, (struct Vec3*)(c + 0x5c), 3);
            Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);

            y1 = ((VObj*)c)->m29() >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);

            Matrix4x3_ApplyInPlaceToRotationZXYExt(&data_020a0e68,
                *(s16*)(c + 0x8c), *(s16*)(c + 0x8e), *(s16*)(c + 0x90));

            y2 = (-((VObj*)c)->m29()) >> 3;
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);

            *(struct Mtx43*)(c + 0xf0) = data_020a0e68;

            *(int*)(c + 0x360) = *(int*)(c + 0x5c) >> 3;
            *(int*)(c + 0x364) = *(int*)(c + 0x60) >> 3;
            *(int*)(c + 0x368) = *(int*)(c + 0x64) >> 3;

            _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
                c, c + 0x124, c + 0x33c, 0xa0000, 0x2bc000, 0xf);
            return;
        }
    }

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));

    *(int*)(c + 0x114) = (*(int*)(c + 0x5c) + *(int*)(c + 0x378)) >> 3;
    *(int*)(c + 0x118) = (*(int*)(c + 0x60) + 0x3c000) >> 3;
    *(int*)(c + 0x11c) = (*(int*)(c + 0x64) + *(int*)(c + 0x380)) >> 3;

    if (*(int*)(c + 0x394) == 0 || *(int*)(c + 0x38c) == 2 || *(int*)(c + 0x38c) == 5) {
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x124, c + 0xf0, 0x82000, 0x2bc000, 0xf);
    }
}
