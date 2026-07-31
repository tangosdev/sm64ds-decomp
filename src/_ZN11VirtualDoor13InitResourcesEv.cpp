//cpp
extern "C" {
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;
extern int IsStarCollectedInLevel(signed char levelID, int starID);
extern void Matrix4x3_FromTranslation(void* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void* m, short angY);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short angX);
extern void InvMat4x3(void* out, void* in);

#define M(p) ((int)(((long long)(int)(p))))

int _ZN11VirtualDoor13InitResourcesEv(char* c)
{
    if (data_0209f2f8 == 8 && data_0209f220 != 1) {
        if (IsStarCollectedInLevel(8, 1) != 0) return 0;
    }

    if ((unsigned char)((unsigned int)*(int*)(c + 8) >> 24) == 0x12) {
        if (IsStarCollectedInLevel(0x12, 1) != 0) {
            *(int*)M(c + 0x5c) += 0x802000;
        }
    }

    if (data_0209f2f8 == 0x20) {
        *(int*)(c + 0x80) = 0x2bc0000;
        *(int*)(c + 0x84) = 0x3200000;
    } else if (data_0209f2f8 == 0x22) {
        *(int*)(c + 0x80) = 0x3e80000;
        *(int*)(c + 0x84) = 0x3e80000;
    } else {
        *(int*)(c + 0x80) = (unsigned int)(((*(int*)(c + 8) & 0xf) + 1) * 0x64000) >> 1;
        *(int*)(c + 0x84) = (((*(unsigned int*)(c + 8) >> 4) & 0xf) + 1) * 0x64000;
    }

    if (*(short*)(c + 0x8c) == 0) {
        *(int*)M(c + 0x80) += 0x64000;
        *(int*)M(c + 0x84) += 0x64000;
        *(int*)M(c + 0x60) -= 0x32000;
    }

    Matrix4x3_FromTranslation(c + 0xd4, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    Matrix4x3_ApplyInPlaceToRotationY(c + 0xd4, -*(short*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(c + 0xd4, -*(short*)(c + 0x8c));
    InvMat4x3(c + 0xd4, c + 0xd4);

    *(int*)M(c + 0x60) += *(int*)(c + 0x84) >> 1;
    return 1;
}
}
