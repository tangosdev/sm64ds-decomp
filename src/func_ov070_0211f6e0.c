typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern u32 data_ov070_02123510[];
extern s8 data_0209f2f8;
extern char data_ov070_0212359c;
extern char data_ov070_021235dc;
extern s32 data_0209f32c;
extern s32 data_020a0e68[];

extern int ApproachAngle(s16* angle, int target, int step, int maxDelta, int minDelta);
extern int _ZN9Animation8FinishedEv(void* thiz);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* thisp, void* bca, int a, int fix, unsigned int j);
extern void FlyGuy_ChangeState(void* c, void* p);
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void* thiz);
extern void* _ZN8dActor_c22ClosestNonVanishPlayerEv(void* thiz);
extern s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
extern Fix12 Vec3_Dist(const Vector3* a, const Vector3* b);
extern u16 DecIfAbove0_Short(u16* p);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* mF, s16 angX);
extern void MulVec3Mat4x3(const Vector3* v, void* m, Vector3* res);

int func_ov070_0211f6e0(char* c)
{
    char* player;
    Vector3 tmp;
    Vector3 v;
    Vector3 aim;
    s16 vAngle;
    s16 half;
    s32 z;

    ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x3e6), 0x100, 0x1000, 0x1000);
    ApproachAngle((s16*)(c + 0x96), 0, 0x100, 0x1000, 0x1000);

    if (_ZN9Animation8FinishedEv(c + 0x350)) {
        if (*(s32*)(c + 0x3d8) == 0) {
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c + 0x300, (void*)data_ov070_02123510[1], 0, 0x1000, 0);
            *(s32*)(c + 0x3d8) = 1;
        }
        if (*(s32*)(c + 0x3dc) == 1) {
            if (data_0209f2f8 != 0x16)
                *(s32 *)(c + 0x3c4) += 0x12c000;
            *(s32*)(c + 0x3d8) = 0;
            *(u16*)(c + 0x3cc) = 0x5a;
            FlyGuy_ChangeState(c, &data_ov070_0212359c);
            return 1;
        }
    }

    if (*(u16*)(c + 0x100) == 0 || _ZNK10dBgCh_Actr8IsOnWallEv(c + 0x144)) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    player = (char*)_ZN8dActor_c22ClosestNonVanishPlayerEv(c);
    if (player == 0) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
        }
        *(u16*)(c + 0x92) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    if (*(u8*)(player + 0x706) != 0 && data_0209f32c > *(s32*)(c + 0x60)) {
        if (data_0209f2f8 != 0x16) {
            *(s32*)(c + 0x3c0) = *(s32*)(c + 0x5c);
            *(s32*)(c + 0x3c4) = *(s32*)(c + 0x60);
            *(s32*)(c + 0x3c8) = *(s32*)(c + 0x64);
            *(s32 *)(c + 0x3c4) += 0xc8000;
        }
        *(s32*)(c + 0x5c) = *(s32*)(c + 0x68);
        *(s32*)(c + 0x60) = *(s32*)(c + 0x6c);
        *(s32*)(c + 0x64) = *(s32*)(c + 0x70);
        *(u16*)(c + 0x92) = 0;
        *(u16*)(c + 0x100) = 0;
        *(u32*)(c + 0xa8) = 0;
        FlyGuy_ChangeState(c, &data_ov070_021235dc);
        return 1;
    }

    tmp = *(Vector3 *)(player + 0x5c);
    z = 0;
    v.x = z;
    v.y = z;
    v.z = z;
    tmp.y = *(int *)(player + 0x644);
    if (data_0209f2f8 == 0x16)
        tmp.y += 0x32000;
    else
        tmp.y += 0x47000;
    {
        int tx = tmp.x;
        int ty = tmp.y;
        int tz = tmp.z;
        aim.x = tx;
        aim.y = ty;
        aim.z = tz;
    }
    vAngle = Vec3_VertAngle((Vector3*)(c + 0x5c), &aim);
    ApproachAngle((s16*)(c + 0x92), vAngle, 0xa, 0x200, 0x100);

    v.z = 0x11000;
    if (*(s32*)(c + 0x60) <= *(s32*)((char*)&tmp + 4) + 0x5000 ||
        *(s32*)(c + 0x60) <= *(s32*)(player + 0x60) + 0x5000 ||
        Vec3_Dist((Vector3*)(c + 0x5c), (Vector3*)(c + 0x3c0)) > 0x5dc000) {
        DecIfAbove0_Short((u16*)(c + 0x100));
        v.z = 0x9000;
    }

    half = (*(s16*)(c + 0x94) - *(s16*)(c + 0x3e6)) / 2;
    ApproachAngle((s16*)(c + 0x96), half, 0xa, 0x100, 0x50);

    Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16*)(c + 0x92));
    MulVec3Mat4x3(&v, data_020a0e68, (Vector3*)(c + 0xa4));

    return 1;
}
