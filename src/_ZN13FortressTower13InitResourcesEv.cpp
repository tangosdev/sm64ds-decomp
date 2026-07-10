//cpp
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;
struct Matrix4x3;
struct CLPS_Block;

extern "C" void *Model_LoadFile(void *fp);
extern "C" void ModelBase_SetFile(void *self, void *bmd, int a, int b);
extern "C" void Platform_UpdateModelPosAndRotY(void *self);
extern "C" void Platform_UpdateClsnPosAndRot(void *self);
extern "C" void *MeshCollider_LoadFile(void *fp);
extern "C" void MovingMeshCollider_SetFile(void *self, void *kcl, void *mtx, int fix, short s, void *clps);
extern "C" int IsStarCollectedInCurLevel(int a);

extern void *data_ov102_0214e188[];
extern void *data_ov102_0214e18c[];
extern void *data_ov102_0214e190[];
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

extern "C" int _ZN13FortressTower13InitResourcesEv(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned short id = *(unsigned short *)(c + 0xc);
    unsigned char idx;

    switch (id) {
    case 0x31:  *(unsigned char *)(c + 0x31e) = 2; break;
    case 0x3b:  *(unsigned char *)(c + 0x31e) = 1; break;
    case 0x3d:  *(unsigned char *)(c + 0x31e) = 0; break;
    case 0x12a: *(unsigned char *)(c + 0x31e) = 3; break;
    case 0x154: *(unsigned char *)(c + 0x31e) = 4; break;
    case 0x155: *(unsigned char *)(c + 0x31e) = 5; break;
    }

    idx = *(unsigned char *)(c + 0x31e);
    {
        void *f = Model_LoadFile(*(void **)((char *)data_ov102_0214e188 + idx * 0xc));
        ModelBase_SetFile(c + 0xd4, f, 1, -1);
    }
    Platform_UpdateModelPosAndRotY(c);
    Platform_UpdateClsnPosAndRot(c);

    idx = *(unsigned char *)(c + 0x31e);
    if (idx == 0 || idx == 2) {
        void *f = MeshCollider_LoadFile(*(void **)((char *)data_ov102_0214e18c + idx * 0xc));
        MovingMeshCollider_SetFile(c + 0x124, f, c + 0x2ec, 0x1000,
                                   *(short *)(c + 0x8e),
                                   *(void **)((char *)data_ov102_0214e190 + idx * 0xc));
    } else {
        void *f = MeshCollider_LoadFile(*(void **)((char *)data_ov102_0214e18c + idx * 0xc));
        MovingMeshCollider_SetFile(c + 0x124, f, c + 0x2ec, 0x199,
                                   *(short *)(c + 0x8e),
                                   *(void **)((char *)data_ov102_0214e190 + idx * 0xc));
    }

    {
        int b31 = (int)(*(unsigned short *)(c + 0xc) == 0x31);
        if (b31 != 0 && data_0209f2f8 == 7) {
            if (data_0209f220 != 1) {
                if (IsStarCollectedInCurLevel(1) != 0)
                    goto ret1;
            }
            if (*(int *)(c + 0x60) >= 0xdac000)
                return 0;
        }
    }
ret1:
    return 1;
}
