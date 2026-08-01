#include "types.h"
typedef struct { s32 x, y, z; } Vec3;

void* _ZN5Actor22ClosestNonVanishPlayerEv(void* self);
s16 Vec3_HorzAngle(void* a, void* b);
void ApproachAngle(s16* p, s16 target, int a, int b, int max);
void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, void* pos, void* rot, int e, int f);
void func_02012694(int a, void* p);
void Matrix4x3_FromRotationY(void* m, int angle);
void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 ax);
void MulVec3Mat4x3(void* in, void* m, void* out);
int _ZN9Animation8FinishedEv(void* anim);
s32 Vec3_Dist(void* a, void* b);
void func_ov065_0211691c(void* self, void* p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* bca, int a, int f, u32 e);

extern int data_020a0e68;
struct SFP { void* a; void* b; };
extern struct SFP data_ov065_0211d600;
extern int data_ov065_0211d670;
extern int data_ov065_0211d680;

int func_ov065_02116364(void* self)
{
    u8* c = (u8*)self;
    Vec3 pp;
    Vec3 spv;
    Vec3 sout;
    Vec3 d;
    Vec3 tgt;
    u8* pl = (u8*)_ZN5Actor22ClosestNonVanishPlayerEv(self);
    if (pl != 0) {
        pp = *(Vec3*)(pl + 0x5c);
        tgt.x = pp.x;
        tgt.y = pp.y;
        tgt.z = pp.z;
        *(s16*)(c + 0x3e0) = Vec3_HorzAngle((void*)(c + 0x5c), &tgt);
        ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x3e0), 1, 0x500, 0x500);
        if (((*(u32*)(c + 0x358) << 4) >> 16) >= 0xf
            && *(u16*)(c + 0x100) == 0
            && *(s32*)(c + 0x3dc) < 3) {
            void* spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xe9, 1, (void*)(c + 0x3c0), 0, *(signed char*)(c + 0xcc), -1);
            if (spawned != 0) {
                u8* sp2 = (u8*)spawned;
                s32 *cnt;
                s32 o;
                func_02012694(0xfb, (void*)(c + 0x74));
                o = 0;
                spv.x = o;
                spv.y = o;
                spv.z = 0x1e000;
                sout.x = o;
                sout.y = o;
                sout.z = o;
                Matrix4x3_FromRotationY(&data_020a0e68, *(s16*)(c + 0x8e));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(s16*)(c + 0x8c));
                MulVec3Mat4x3(&spv, &data_020a0e68, &sout);
                *(s32*)(sp2 + 0xa4) = sout.x;
                cnt = (s32*)((s64)(s32)(c + 0x3dc));
                *(s32*)(sp2 + 0xa8) = sout.y;
                *(s32*)(sp2 + 0xac) = sout.z;
                *cnt = *cnt + 1;
                *(u16*)(c + 0x100) = 4;
            }
        }
    }
    if (_ZN9Animation8FinishedEv((void*)(c + 0x350)) != 0) {
        if (pl != 0) {
            s32 *bp = (s32*)((s64)(s32)(pl + 0x5c));
            d.x = bp[0];
            d.y = bp[1];
            d.z = bp[2];
            if (Vec3_Dist((void*)(c + 0x5c), &d) > 0x3e8000) {
                *(u16*)(c + 0x100) = 0;
                func_ov065_0211691c(self, &data_ov065_0211d670);
            } else {
                *(u16*)(c + 0x100) = 0x32;
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj((void*)(c + 0x300), data_ov065_0211d600.b, 0, 0x1000, 0);
                func_ov065_0211691c(self, &data_ov065_0211d680);
            }
        } else {
            *(u16*)(c + 0x100) = 0;
            func_ov065_0211691c(self, &data_ov065_0211d670);
        }
    }
    return 1;
}
