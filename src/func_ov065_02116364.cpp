//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vec3 { s32 x, y, z; };
struct V3A { int w[3]; };

extern "C" {
    void* _ZN5Actor22ClosestNonVanishPlayerEv(void* self);
    s16 Vec3_HorzAngle(void* a, void* b);
    void ApproachAngle(void* p, s16 target, int a, int b, int e);
    void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, void* pos, void* rot, int e, int f);
    void func_02012694(int a, void* p);
    void Matrix4x3_FromRotationY(void* m, int angle);
    void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 ax);
    void MulVec3Mat4x3(void* in, void* m, void* out);
    int _ZN9Animation8FinishedEv(void* anim);
    s32 Vec3_Dist(void* a, void* b);
    void func_ov065_0211691c(void* self, void* p);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, u32 e);
}

extern int data_020a0e68;
struct SFP { void* a; void* b; };
extern SFP data_ov065_0211d600;
extern int data_ov065_0211d670;
extern int data_ov065_0211d680;

extern "C" int func_ov065_02116364(void* self)
{
    u8* c = (u8*)self;
    struct { Vec3 pp; Vec3 spv; Vec3 sout; Vec3 d; Vec3 tgt; } L;
    u8* pl = (u8*)_ZN5Actor22ClosestNonVanishPlayerEv(self);
    if (pl != 0) {
        *(V3A*)(int)(((long long)(int)&L.pp) & 0xFFFFFFFFFFFFFFFFLL) = *(V3A*)(int)(((long long)(int)(pl + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        L.tgt = L.pp;
        *(s16*)(c+0x3e0) = Vec3_HorzAngle((void*)(c+0x5c), &L.tgt);
        ApproachAngle((void*)(c+0x94), *(s16*)(c+0x3e0), 1, 0x500, 0x500);

        if (((*(u32*)(c+0x358) << 4) >> 16) >= 0xf
            && *(u16*)(c+0x100) == 0
            && *(s32*)(c+0x3dc) < 3) {
            void* spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xe9, 1, (void*)(c+0x3c0), 0, *(signed char*)(c+0xcc), -1);
            if (spawned != 0) {
                u8* sp2 = (u8*)spawned;
                func_02012694(0xfb, (void*)(c+0x74));
                L.spv.x = 0;
                L.spv.y = 0;
                L.spv.z = 0x1e000;
                L.sout.x = 0; L.sout.y = 0; L.sout.z = 0;
                Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c+0x8e));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c+0x8c));
                MulVec3Mat4x3(&L.spv, &data_020a0e68, &L.sout);
                *(s32*)(sp2+0xa4) = L.sout.x;
                *(s32*)(sp2+0xa8) = L.sout.y;
                *(s32*)(sp2+0xac) = L.sout.z;
                *(s32*)(int)(((long long)(int)(c + 0x3dc)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
                *(u16*)(c+0x100) = 4;
            }
        }
    }

    if (_ZN9Animation8FinishedEv((void*)(c+0x350)) != 0) {
        if (pl != 0) {
            s32* dsrc = (s32*)(int)(((long long)(int)(pl + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
            L.d.x = dsrc[0];
            L.d.y = dsrc[1];
            L.d.z = dsrc[2];
            if (Vec3_Dist((void*)(c+0x5c), &L.d) > 0x3e8000) {
                *(u16*)(c+0x100) = 0;
                func_ov065_0211691c(self, &data_ov065_0211d670);
            } else {
                *(u16*)(c+0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c+0x300), data_ov065_0211d600.b, 0, 0x1000, 0);
                func_ov065_0211691c(self, &data_ov065_0211d680);
            }
        } else {
            *(u16*)(c+0x100) = 0;
            func_ov065_0211691c(self, &data_ov065_0211d670);
        }
    }
    return 1;
}
