//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef struct { s32 m[12]; } Matrix4x3;

extern "C" {
extern int _ZN5Sound20PlaySmallSecretSoundEP5ActorPt(void* actor, u16* arg);
extern void Matrix4x3_FromRotationY(void* m, int angle);
extern void MulVec3Mat4x3(const Vector3* v, const Matrix4x3* m, Vector3* res);
extern void Vec3_Add(Vector3* out, const Vector3* a, const Vector3* b);
extern int _ZN6Player12GetTalkStateEv(void* p);
extern int _Z14ApproachLinearRsss(s16* val, s16 target, s16 step);
extern void _ZN7Message11PrepareTalkEv(void);
extern void _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* p, void* base, u32 a, const Vector3* v, u32 b, u32 c);
extern void _ZN7Message7EndTalkEv(void);
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern int AngleDiff(int a, int b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void* p, void* base, int b);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
}

extern Matrix4x3 MATRIX_SCRATCH_PAPER;

struct Obj {
    char pad0[0x5c];
    Vector3 pos;        /* 0x5c */
    char pad1[0x8e - 0x68];
    s16 angle_8e;       /* 0x8e */
    char pad2[0x124 - 0x90];
    u32 clsn_124;       /* 0x124 CylinderClsn */
    u32 f128;           /* 0x128 */
    char pad3[0x144 - 0x12c];
    u32 f144;           /* 0x144 */
    u32 f148;           /* 0x148 */
    char pad4[0x158 - 0x14c];
    Vector3 vec_158;    /* 0x158 */
    s32 f164;           /* 0x164 */
    u16 arr_168[4];     /* 0x168 */
};

extern "C" int func_ov010_02111ae0(Obj* thiz)
{
    Vector3 vIn, vMid, vRes;
    Vector3 hv;
    Vector3 vIn2, vMid2, vRes2;

    if (thiz->f164 != 0) {
        void* p;
        if (!_ZN5Sound20PlaySmallSecretSoundEP5ActorPt(thiz, thiz->arr_168))
            return 1;
        p = (void*)thiz->f164;

        vIn.x = -0x3b0000;
        vIn.y = 0x200000;
        vIn.z = 0x120000;
        vMid.x = 0;
        vMid.y = 0;
        vMid.z = 0;
        Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, thiz->angle_8e);
        MulVec3Mat4x3(&vIn, &MATRIX_SCRATCH_PAPER, &vMid);
        Vec3_Add(&vRes, &thiz->pos, &vMid);

        switch (_ZN6Player12GetTalkStateEv(p)) {
        case 0:
            if (_Z14ApproachLinearRsss((s16*)((char*)p + 0x8e), thiz->angle_8e + 0x8000, 0x800)) {
                _ZN7Message11PrepareTalkEv();
                _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(p, thiz, 0x192, &vRes, 0, 0);
            }
            break;
        case 1:
            break;
        default:
            _ZN7Message7EndTalkEv();
            thiz->f164 = 0;
            break;
        }
    } else {
        if (thiz->f144 & 0x8000000) {
            void* o = _ZN5Actor10FindWithIDEj(thiz->f148);
            if (o) {
                int b = (int)(*(u16*)((char*)o + 0xc) == 0xbf);
                if (b) {
                    Vector3* op = (Vector3*)(((int)o + 0x5c) & 0xFFFFFFFFFFFFFFFFull);
                    hv = *op;
                    if (AngleDiff(Vec3_HorzAngle(&thiz->pos, &hv), thiz->angle_8e) < 0x4000) {
                        if (*(s32*)((char*)o + 0x664) == 0xd) {
                            if (_ZN6Player9StartTalkER9ActorBaseb(o, thiz, 0)) {
                                thiz->f164 = (s32)o;
                                *(s16*)((char*)thiz + 0x168) = 0;
                            }
                        }
                    }
                }
            }
        }
    }

    vIn2.x = -0x3b0000;
    vIn2.y = 0;
    vIn2.z = 0x320000;
    vMid2.x = 0;
    vMid2.y = 0;
    vMid2.z = 0;
    Matrix4x3_FromRotationY(&MATRIX_SCRATCH_PAPER, thiz->angle_8e);
    MulVec3Mat4x3(&vIn2, &MATRIX_SCRATCH_PAPER, &vMid2);
    Vec3_Add(&vRes2, &thiz->pos, &vMid2);
    thiz->vec_158.x = vRes2.x;
    thiz->vec_158.y = vRes2.y;
    thiz->vec_158.z = vRes2.z;
    thiz->f128 = 0xf0000;
    _ZN12CylinderClsn5ClearEv(&thiz->clsn_124);
    _ZN12CylinderClsn6UpdateEv(&thiz->clsn_124);
    return 1;
}
