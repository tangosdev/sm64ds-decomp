//cpp
typedef unsigned int u32;
typedef unsigned char u8;
typedef short s16;

typedef struct Mtx43 { int w[12]; } Mtx43;

extern "C" void Matrix4x3_FromRotationY(void* m, int angle);
extern "C" void _ZN9ModelBase12ApplyOpacityEj(void* self, u32 op, int z);
extern "C" void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern "C" void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
extern "C" void Matrix4x3_ApplyInPlaceToRotationY(void* m, s16 angY);
extern "C" void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* c, void* shadow, void* mtx, int rad, int height, u32 flags);

extern Mtx43 data_020a0e68;

struct Vec3i { int x, y, z; };
extern Vec3i data_ov081_02128ef8;

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

extern "C" void func_ov081_02126a20(char* c)
{
    int b = (int)((*(u32*)(c + 0xb0) & 0x40000) != 0);
    if (b)
        return;

    Matrix4x3_FromRotationY(c + 0xf0, *(s16*)(c + 0x8e));
    *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;

    _ZN9ModelBase12ApplyOpacityEj(c + 0xd4, *(u8*)(c + 0x3f0), 0);

    if (*(s16*)(c + 0x8c) != 0) {
        data_020a0e68 = *(Mtx43*)(c + 0xf0);
        int y1 = ((VObj*)c)->m29() >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y1, 0);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x8c));
        int y2 = (-((VObj*)c)->m29()) >> 3;
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0, y2, 0);
        *(Mtx43*)(c + 0xf0) = data_020a0e68;
    } else {
        data_020a0e68 = *(Mtx43*)(c + 0xf0);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, data_ov081_02128ef8.x, data_ov081_02128ef8.y, data_ov081_02128ef8.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(s16*)(c + 0x3ec));
        *(Mtx43*)(c + 0x154) = data_020a0e68;
    }

    _ZN9ModelBase12ApplyOpacityEj(c + 0x138, (0x20 - *(u8*)(c + 0x3f0)) & 0xff, 0);

    *(int*)(c + 0x3c4) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x3c8) = *(int*)(c + 0x60) >> 3;
    *(int*)(c + 0x3cc) = *(int*)(c + 0x64) >> 3;

    int dh = (*(int*)(c + 0x3e0) == 8) ? 0x258000 : 0x12c000;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x188, c + 0x3a0, 0x78000, dh, 0xf);
}
