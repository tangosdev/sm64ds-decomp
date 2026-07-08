//cpp
// NONMATCHING: different op / idiom (div=60). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;
struct BCA_File;

extern struct { u32 d[4]; } data_ov073_021232a8;
extern char data_ov073_02123340;
extern char data_ov073_02123320;

extern "C" {
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, BCA_File* f, int a, int b, Fix12 fx, u16 t);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern s16 Vec3_VertAngle(const Vector3* v1, const Vector3* v0);
extern void _Z14ApproachLinearRsss(s16* val, s16 target, s16 step);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, s16 angX);
extern void MulVec3Mat4x3(const Vector3* v, const void* m, Vector3* res);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* thiz);
extern void func_ov073_0211f2c0(void* c, Fix12 f);
extern void func_02012694(int a, void* b);
extern void func_ov073_0212157c(void* c, void* p);
}

extern char MATRIX_SCRATCH_PAPER;

extern "C" int func_ov073_021203ac(char* c)
{
    u16 state = *(u16*)(c + 0x100);
    if (state == 0)
        goto state0;
    if (state != 1)
        goto ret1;

    /* state 1 */
    *(s32*)(c + 0xa8) = 0x5a000;
    *(s32*)(c + 0x9c) = -0x5000;
    {
        u8* p = (u8*)(c + 0x4c4);
        *p = *p + 1;
        *p = *p & 7;
    }
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(
        c + 0x30c, (BCA_File*)data_ov073_021232a8.d[1], 4, 0, 0x1000, 0);
    *(s32*)(c + 0x368) = 0x2000;
ret1:
    return 1;

state0:
    {
        Vector3 dir;
        s16 ha, va;
        if (*(u8*)(c + 0x4cb) == 2) {
            Vector3* src = (Vector3*)(c + 0x448 + *(u8*)(c + 0x4c4) * 0xc);
            dir.x = src->x;
            dir.y = src->y;
            dir.z = src->z;
        } else {
            Vector3* src = (Vector3*)(c + 0x3e8 + *(u8*)(c + 0x4c4) * 0xc);
            dir.x = src->x;
            dir.y = src->y;
            dir.z = src->z;
        }
        ha = Vec3_HorzAngle((Vector3*)(c + 0x5c), &dir);
        va = Vec3_VertAngle((Vector3*)(c + 0x5c), &dir);
        _Z14ApproachLinearRsss((s16*)(c + 0x94), ha, 0x800);

        {
            Vector3 vIn, vOut;
            vIn.x = 0;
            vIn.y = 0;
            vIn.z = 0x50000;
            vOut.x = 0;
            vOut.y = 0;
            vOut.z = 0;
            Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, ha);
            Matrix4x3_ApplyInPlaceToRotationX(&MATRIX_SCRATCH_PAPER, va);
            MulVec3Mat4x3(&vIn, &MATRIX_SCRATCH_PAPER, &vOut);
            *(s32*)(c + 0xa4) = vOut.x;
            *(s32*)(c + 0xac) = vOut.z;
        }

        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x150)) {
            *(u8*)(c + 0x4c5) = *(u8*)(c + 0x4c4);
            *(s32*)(c + 0x98) = 0;
            *(s32*)(c + 0xa4) = 0;
            *(s32*)(c + 0xa8) = 0;
            *(s32*)(c + 0xac) = 0;
            func_ov073_0211f2c0(c, 0x7d0000);
            func_02012694(0x16d, c + 0x74);
            {
                s32* cnt = (s32*)(c + 0x4b8);
                *cnt = *cnt + 1;
            }
            if (*(s32*)(c + 0x4b8) > 7)
                func_ov073_0212157c(c, &data_ov073_02123340);
            else
                func_ov073_0212157c(c, &data_ov073_02123320);
        }
    }

    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    return 1;
}
