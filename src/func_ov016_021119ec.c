typedef short s16;

struct Vector3 { int x, y, z; };

extern int Vec3_Dist(const void* a, const void* b);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void* pos, const void* target, int step);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern void Matrix4x3_FromRotationY(void* m, int ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, int ang);
extern void MulVec3Mat4x3(const void* in, const void* m, void* out);
extern s16 _ZN5Actor18HorzAngleToCPlayerEv(void* self);
extern int AngleDiff(int a, int b);
extern int Vec3_HorzDist(const void* a, const void* b);
extern void func_ov016_02111bf0(void* self, void* state);

extern unsigned char data_0209f220;
extern int data_020a0e68[];
extern void* data_ov016_02114d9c;

#pragma opt_propagation off
#define LA(p) ((int*)(unsigned long)(((long long)(int)(unsigned long)(p)) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov016_021119ec(char* c)
{
    void* player;
    struct Vector3 playerPos;
    struct Vector3 local;
    struct Vector3 world;
    int thrHorz;
    int thrVert;
    int thrAng;
    unsigned char stage;
    int zero;
    int dy;

    if (Vec3_Dist(c + 0x5c, c + 0x3f0) > 0xa000) {
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x5c, c + 0x3f0, 0x5000);
        return 1;
    }

    *(int*)(c + 0x5c) = *(int*)(c + 0x3f0);
    *(int*)(c + 0x60) = *(int*)(c + 0x3f4);
    *(int*)(c + 0x64) = *(int*)(c + 0x3f8);

    player = _ZN5Actor13ClosestPlayerEv(c);
    if (player != 0) {
        int* p = LA((char*)player + 0x5c);

        /* ACDB order: x,y,z then stage — colors y/z as r2 and stage as r1 */
        playerPos.x = p[0];
        playerPos.y = p[1];
        playerPos.z = p[2];
        stage = data_0209f220;

        thrHorz = 0x3e8000;
        thrAng = 0x3300;
        thrVert = 0x418000;
        thrVert += 0x3e8000;

        if (stage == 1) {
            zero = 0;
            local.z = zero;
            local.z = 0x64000;
            local.x = zero;
            local.y = zero;
            world.x = zero;
            world.y = zero;
            world.z = zero;
            thrHorz = 0x495000;
            thrVert = 0x6ee000;
            Matrix4x3_FromRotationY(data_020a0e68, *(s16*)(c + 0x8e));
            Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(s16*)(c + 0x8c));
            MulVec3Mat4x3(&local, data_020a0e68, &world);
            world.x += *(int*)(c + 0x3f0);
            world.y += *(int*)(c + 0x3f4);
            world.z += *(int*)(c + 0x3f8);
            if (Vec3_Dist(&playerPos, &world) < 0x224000)
                func_ov016_02111bf0(c, &data_ov016_02114d9c);
        }

        if (AngleDiff(_ZN5Actor18HorzAngleToCPlayerEv(c), *(s16*)(c + 0x8e)) < thrAng) {
            if (Vec3_HorzDist(c + 0x5c, &playerPos) < thrHorz) {
                dy = *(int*)(c + 0x60) - playerPos.y;
                if (dy < 0)
                    dy = -dy;
                if (dy < thrVert)
                    func_ov016_02111bf0(c, &data_ov016_02114d9c);
            }
        }
    }
    return 1;
}
