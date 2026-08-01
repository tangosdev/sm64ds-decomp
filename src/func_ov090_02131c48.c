typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef struct { int x, y, z; } Vector3;

extern void func_02012694(unsigned int id, const Vector3* v);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* c);
extern int AngleDiff(int a, int b);
extern int ApproachAngle(s16* cur, int target, int divisor, int band, int maxStep);
extern int _ZN9Animation8FinishedEv(void* c);
extern void func_ov090_02131e00(void* c, const void* p);
extern int RandomIntInternal(int* seed);

extern int data_ov090_02134504;
extern int data_0209e650;
extern int data_ov090_021344e4;

int func_ov090_02131c48(char* c)
{
    if ((((unsigned)*(int*)(c + 0x364)) << 4) >> 16 >= 4)
        *(int*)(c + 0x98) = 0x19000;

    if ((((unsigned)*(int*)(c + 0x364)) << 4) >> 16 == 4)
        func_02012694(0xfd, (const Vector3*)(c + 0x74));

    if (*(u16*)(c + 0x394) == 0
        && _ZNK12WithMeshClsn8IsOnWallEv(c + 0x150)
        && AngleDiff(*(s16*)(c + 0x94), *(s16*)(c + 0x39a)) < 0x200) {
        s16* p = (s16*)(((int)c + 0x39a));
        *p = *p + 0x4000;
        *(s16*)(c + 0x394) = 8;
    }

    if (*(u16*)(c + 0x394) != 0) {
        ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x39a), 1, 0x1000, 0x1000);
    } else {
        if (_ZN9Animation8FinishedEv(c + 0x35c)) {
            int* q = (int*)(((int)c + 0x390));
            *q = *q + 1;
            if (*(int*)(c + 0x390) > 0x14)
                func_ov090_02131e00(c, &data_ov090_02134504);
        }
    }

    if (*(u8*)(c + 0x39c) == 0) {
        *(int*)(c + 0x374) = *(int*)(c + 0x5c);
        *(int*)(c + 0x378) = *(int*)(c + 0x60);
        *(int*)(c + 0x37c) = *(int*)(c + 0x64);
        *(s16*)(c + 0x39a) = (s16)(((unsigned)RandomIntInternal(&data_0209e650) >> 8) << 13);
        func_ov090_02131e00(c, &data_ov090_021344e4);
    }

    return 1;
}
