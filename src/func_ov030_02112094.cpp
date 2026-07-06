//cpp
typedef struct M4x3 { int w[12]; } M4x3;

struct Vec3 { int x, y, z; };
struct Vec3s { short x, y, z; };
struct Bundle { Vec3s rot; short _p; Vec3 trans; int _tail[2]; };

extern "C" {
extern int _ZN6Player14IsFrontSlidingEv(void*);
extern int _ZN6Player17LostGrabbedObjectEv(void*);
extern void* _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(void*, void*, void*);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void* self, void* shadow, void* mtx, int rad, int height, unsigned int flags);
extern void* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void MulMat4x3Mat4x3(void* a, void* b, void* out);
extern void Matrix4x3_ApplyInPlaceToTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void* m, int x, int y, int z);
extern char data_ov030_02115ddc[];
extern M4x3 data_020a0e68;
void func_ov030_02112094(void* self);
}

void func_ov030_02112094(void* self)
{
    char* c = (char*)self;
    int idx;
    void* res;
    void* obj;
    unsigned int id;
    Bundle bnd;

    int a = (int)((*(int*)(c + 0xb0) & 0x100) != 0);
    if (a && *(void**)(c + 0x3a8)
        && *(int*)(*(char**)(c + 0x3a8) + 0xc8)) {
        idx = 0;
        if (_ZN6Player14IsFrontSlidingEv(*(void**)(c + 0x3a8))
            || _ZN6Player17LostGrabbedObjectEv(*(void**)(c + 0x3a8))) {
            idx = 1;
        }
        if (*(int*)(*(char**)(c + 0x3a8) + 8) == 2) {
            idx = (idx + 2) & 0xff;
        }
        res = _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(c, *(void**)(c + 0x3a8),
            data_ov030_02115ddc + idx * 0xc);
        *(M4x3*)(c + 0xf0) = *(M4x3*)res;
    } else {
        Matrix4x3_FromRotationY(c + 0xf0, *(short*)(c + 0x8e));
        *(int*)(c + 0x114) = *(int*)(c + 0x5c) >> 3;
        *(int*)(c + 0x118) = *(int*)(c + 0x60) >> 3;
        *(int*)(c + 0x11c) = *(int*)(c + 0x64) >> 3;
    }

    int b = (int)((*(int*)(c + 0xb0) & 0x40000) != 0);
    if (!b) {
        _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
            c, c + 0x138, c + 0xf0, 0x5a000, 0x190000, 0xf);
    }

    id = *(unsigned int*)(c + 0x3ac);
    if (id == 0)
        return;

    obj = _ZN5Actor10FindWithIDEj(id);

    bnd.trans.x = 0xa00;
    bnd.trans.y = 0;
    bnd.trans.z = -0x2f00;
    bnd.rot.x = -0x3f00;
    bnd.rot.y = 0;
    bnd.rot.z = -0x4000;

    data_020a0e68 = *(M4x3*)(c + 0xf0);
    MulMat4x3Mat4x3(*(char**)(c + 0xe8) + 0xf0, &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68,
        *(volatile int*)&bnd.trans.x, *(volatile int*)&bnd.trans.y, *(volatile int*)&bnd.trans.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        *(volatile short*)&bnd.rot.x, *(volatile short*)&bnd.rot.y, *(volatile short*)&bnd.rot.z);
    *(M4x3*)(c + 0x350) = data_020a0e68;

    *(int*)((char*)obj + 0xc8) = (int)(c + 0x350);
    *(int*)((char*)obj + 0x5c) = *(int*)(c + 0x5c);
    *(int*)((char*)obj + 0x60) = *(int*)(c + 0x60);
    *(int*)((char*)obj + 0x64) = *(int*)(c + 0x64);
}
