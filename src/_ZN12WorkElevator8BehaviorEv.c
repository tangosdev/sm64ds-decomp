// NONMATCHING: size 0x550 vs target 0x5a4; large structural/regalloc gap (div=size)
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Vec3 { s32 x, y, z; };

extern s16 data_02082214[];
extern u16 data_ov021_02114740[];
extern s32 data_020a0e68[];
extern s32 data_02092768[];

extern u16 DecIfAbove0_Short(u16 *p);
extern u8 DecIfAbove0_Byte(u8 *p);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void *v, u32 d);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(u32 a, void *v);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void MulVec3Mat4x3(void *dst, void *mtx, void *src);
extern void AddVec3(void *a, void *b, void *c);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *actor);
extern s32 _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *c);
extern void Quaternion_SLerp(void *q0, void *q1, int t, void *out);
extern s32 Vec3_Equal(void *a, void *b);
extern void func_ov021_02111434(void *c);
extern void func_ov021_0211129c(void *c);

s32 _ZN12WorkElevator8BehaviorEv(void *arg0)
{
    char *c = (char *)arg0;
    s32 sp4;
    struct Vec3 va[3];
    struct Vec3 vb[3];
    struct Vec3 vc[3];
    struct Vec3 sp74;
    char sp80[0x78];
    char spF8[0x78];
    char sp170[0x78];
    s32 i;
    struct Vec3 *rsb;
    struct Vec3 *r8;
    struct Vec3 *r7;
    char *r7_2;
    char *r5;
    s32 t;

    if (*(u8 *)(c + 0xC7D) != 0) {
        if (DecIfAbove0_Short((u16 *)(c + 0xC78)) != 0) {
            char *b = c + 0xC00;
            u16 *w = (u16 *)(c + 0xC76);
            s32 prod;
            s8 sel;
            s16 v;
            *w = (u16)(*w + 0x1F00);
            v = data_02082214[((u16)*(s16 *)(b + 0x76) >> 4) * 2];
            prod = *(u16 *)(b + 0x78) * 0x1A;
            sel = *(s8 *)(b + 0x7A);
            {
                long long m = (long long)prod * v + 0x800;
                t = (s32)(u32)(m >> 12);
                t = (s16)t;
            }
            switch (sel) {
            case 0:
                *(s16 *)(c + 0x8C) = -t;
                break;
            case 1:
                *(s16 *)(c + 0x8C) = t;
                break;
            case 2:
                *(s16 *)(c + 0x90) = t;
                break;
            case 3:
                *(s16 *)(c + 0x90) = -t;
                break;
            }
        } else {
            s8 s = *(s8 *)(c + 0xC7A);
            if (s == 0 || s == 2)
                *(s8 *)(c + 0xC7A) = (s8)(*(s8 *)(c + 0xC7A) + 1);
            else
                *(s8 *)(c + 0xC7A) = (s8)(*(s8 *)(c + 0xC7A) - 1);
            *(u8 *)(c + 0xC7D) = 0;
            *(u16 *)(c + 0x94) = data_ov021_02114740[*(s8 *)(c + 0xC7A)];
            *(s16 *)(c + 0x8C) = 0;
            *(s16 *)(c + 0x90) = 0;
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0xC44);
        }
        if (*(s8 *)(c + 0xC7B) != *(s8 *)(c + 0xC7A)) {
            *(u8 *)(c + 0xC7D) = 0;
            *(u16 *)(c + 0x94) = data_ov021_02114740[*(s8 *)(c + 0xC7A)];
            *(s16 *)(c + 0x8C) = 0;
            *(s16 *)(c + 0x90) = 0;
            *(s32 *)(c + 0x60) = *(s32 *)(c + 0xC44);
        }
    } else {
        if (*(s8 *)(c + 0xC7A) != -1) {
            *(u32 *)(c + 0xC70) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(u32 *)(c + 0xC70), 3, 0x88, c + 0x74, 0);
            *(u16 *)(c + 0x94) = data_ov021_02114740[*(s8 *)(c + 0xC7A)];
            *(s32 *)(c + 0x98) = 0x5000;
            va[0].x = 0;
            va[0].y = -0xA000;
            va[0].z = 0x10E000;
            va[1].x = 0x12C000;
            va[1].y = -0xA000;
            va[1].z = 0x10E000;
            va[2].x = -0x12C000;
            va[2].y = -0xA000;
            va[2].z = 0x10E000;
            vb[0].x = 0;
            vb[0].y = 0;
            vb[0].z = 0;
            vb[1].x = 0;
            vb[1].y = 0;
            vb[1].z = 0;
            vb[2].x = 0;
            vb[2].y = 0;
            vb[2].z = 0;
            vc[0].x = 0;
            vc[0].y = 0;
            vc[0].z = 0;
            vc[1].x = 0;
            vc[1].y = 0;
            vc[1].z = 0;
            vc[2].x = 0;
            vc[2].y = 0;
            vc[2].z = 0;
            Matrix4x3_FromRotationY(data_020a0e68, *(s16 *)(c + 0x94));
            i = 0;
            rsb = va;
            r8 = vb;
            r7 = vc;
            r5 = c + 0x5C;
            do {
                sp74.x = rsb->x;
                sp74.y = rsb->y;
                sp74.z = rsb->z;
                sp74.z = rsb->z + 0x28000;
                MulVec3Mat4x3(rsb, data_020a0e68, r8);
                MulVec3Mat4x3(&sp74, data_020a0e68, r7);
                AddVec3(r8, r5, r8);
                AddVec3(r7, r5, r7);
                rsb += 1;
                r8 += 1;
                r7 += 1;
                i++;
            } while (i < 3);
            _ZN11RaycastLineC1Ev(sp80);
            _ZN11RaycastLineC1Ev(spF8);
            _ZN11RaycastLineC1Ev(sp170);
            i = 0;
            rsb = vc;
            r8 = vb;
            r7_2 = sp80;
            sp4 = 0x1B;
            do {
                _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(r7_2, r8, rsb, c);
                if (_ZN11RaycastLine10DetectClsnEv(r7_2) != 0 && *(s32 *)(r7_2 + 0x60) <= 0x1F000) {
                    *(s32 *)(c + 0x98) = 0;
                    *(u8 *)(c + 0xC7D) = 1;
                    *(u16 *)(c + 0xC78) = 0x3C;
                    _ZN5Sound9PlayBank3EjRK7Vector3(sp4, c + 0x74);
                }
                i++;
                rsb += 1;
                r8 += 1;
                r7_2 += 0x78;
            } while (i < 3);
            _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
            _ZN11RaycastLineD1Ev(sp170);
            _ZN11RaycastLineD1Ev(spF8);
            _ZN11RaycastLineD1Ev(sp80);
        }
        if (DecIfAbove0_Byte((u8 *)(c + 0xC7C)) == 0) {
            *(s32 *)(c + 0xC5C) = data_02092768[0];
            *(s32 *)(c + 0xC60) = data_02092768[1];
            *(s32 *)(c + 0xC64) = data_02092768[2];
            *(s32 *)(c + 0xC68) = data_02092768[3];
        }
    }
    Quaternion_SLerp(c + 0xC4C, c + 0xC5C, 0x199, c + 0xC4C);
    if (Vec3_Equal(c + 0x5C, c + 0xC40) != 0)
        *(u16 *)(c + 0xC74) = 0x12C;
    if (DecIfAbove0_Short((u16 *)(c + 0xC74)) == 0) {
        *(s32 *)(c + 0x5C) = *(s32 *)(c + 0xC40);
        *(s32 *)(c + 0x60) = *(s32 *)(c + 0xC44);
        *(s32 *)(c + 0x64) = *(s32 *)(c + 0xC48);
        *(s32 *)(c + 0xC4C) = data_02092768[0];
        *(s32 *)(c + 0xC50) = data_02092768[1];
        *(s32 *)(c + 0xC54) = data_02092768[2];
        *(s32 *)(c + 0xC58) = data_02092768[3];
        *(s32 *)(c + 0xC5C) = data_02092768[0];
        *(s32 *)(c + 0xC60) = data_02092768[1];
        *(s32 *)(c + 0xC64) = data_02092768[2];
        *(s32 *)(c + 0xC68) = data_02092768[3];
        *(s8 *)(c + 0xC7A) = -1;
        *(u16 *)(c + 0xC76) = 0;
        *(u8 *)(c + 0xC7D) = 0;
        *(u16 *)(c + 0xC78) = 0;
        *(s16 *)(c + 0x8C) = 0;
        *(s16 *)(c + 0x90) = 0;
        *(s32 *)(c + 0x98) = 0;
    }
    if (*(u8 *)(c + 0xC7D) == 0 && *(u16 *)(c + 0xC74) < 0x10E) {
        *(s8 *)(c + 0xC7A) = -1;
        *(s32 *)(c + 0x98) = 0;
    }
    func_ov021_02111434(c);
    func_ov021_0211129c(c);
    *(u8 *)(c + 0xC7E) = 0;
    *(s32 *)(c + 0x98) = 0;
    if (*(s8 *)(c + 0xC7B) != *(s8 *)(c + 0xC7A))
        _ZN5Sound9PlayBank3EjRK7Vector3(0x3E, c + 0x74);
    *(s8 *)(c + 0xC7B) = *(s8 *)(c + 0xC7A);
    return 1;
}