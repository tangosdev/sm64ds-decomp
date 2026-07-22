//cpp

typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;
typedef signed char s8;

struct Vector3 { int x, y, z; };
struct Matrix4x3 { int m[12]; };

extern "C" {
    s32 Vec3_Dist(const Vector3 *a, const Vector3 *b);
    s32 Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
    void func_0200af20(void *self, s32 *a, s32 *b, s16 *c);
    void func_0200d954(void *self, s16 arg);
    void Matrix4x3_FromRotationZ(void *m, int angle);
    void MulMat4x3Mat4x3(void *dst, void *a, void *b);

    extern s32 data_0209fc48;
    extern u8 data_0208738c;
    extern void *data_ov002_0210c3b0;
    extern void *data_ov002_0210c3e0[2];
    extern u8 data_0209f20c;
    extern u8 data_0209f294;
    extern u8 data_0209f2c4;
    extern s8 data_0209f2f8;
    extern s32 data_0209ee90[];
    extern s16 data_02082214[];
    extern void *data_02086efc;
    extern void *data_02086f08;
    extern Matrix4x3 data_020a0e68;
}

struct OamAttr;

struct OAM {
    static void Render(bool, OamAttr *, int, int, int, int, int, int, int, int);
};
struct G2x {
    static void SetBlendAlpha(volatile unsigned short *, u16, u16, u16, u16);
};
struct G3i {
    static void PerspectiveW_(int, int, int, int, int, int, bool, Matrix4x3 *);
    static void LookAt_(const Vector3 *, const Vector3 *, const Vector3 *, bool, Matrix4x3 *);
};

struct View {
    int render();
    int Render();
};
struct Camera : View {
    int Render();
};

int Camera::Render()
{
    Camera *self = this;
    s32 sp[6];
    s16 sp18;
    s16 var_r4;

    sp[0] = *(s32 *)((char *)self + 0x80);
    sp[1] = *(s32 *)((char *)self + 0x84);
    sp[2] = *(s32 *)((char *)self + 0x88);
    sp[3] = *(s32 *)((char *)self + 0x8c);
    sp[4] = *(s32 *)((char *)self + 0x90);
    sp[5] = *(s32 *)((char *)self + 0x94);
    sp18 = *(s16 *)((char *)self + 0x17a);
    var_r4 = *(s16 *)((char *)self + 0x178);

    if (Vec3_Dist((Vector3 *)&sp[0], (Vector3 *)((char *)self + 0xc8)) < 0x1000 &&
        Vec3_Dist((Vector3 *)&sp[3], (Vector3 *)((char *)self + 0xd4)) < 0x1000) {
        sp[0] = *(s32 *)((char *)self + 0xc8);
        sp[1] = *(s32 *)((char *)self + 0xcc);
        sp[2] = *(s32 *)((char *)self + 0xd0);
        sp[3] = *(s32 *)((char *)self + 0xd4);
        sp[4] = *(s32 *)((char *)self + 0xd8);
        sp[5] = *(s32 *)((char *)self + 0xdc);
    }

    if (!(data_0209fc48 != 0 ? 1 : 0)) {
        s32 f = *(s32 *)((char *)self + 0x154);
        if (!(f & 8)) {
            if (*(u8 **)((char *)self + 0x13c) == &data_0208738c) {
                OAM::Render(0, (OamAttr *)&data_ov002_0210c3b0, 0x80, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                G2x::SetBlendAlpha((volatile unsigned short *)0x04000050, 0x10, 0x2f, 0xc, 6);
            } else {
                if (f & 0x20) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[0], 0x14, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                }
                if (*(s32 *)((char *)self + 0x154) & 0x40) {
                    OAM::Render(0, (OamAttr *)data_ov002_0210c3e0[1], 0xec, 0x60, -1, -1, 0x1000, 0x1000, 0, -1);
                }
            }
            s32 t = *(s32 *)((char *)self + 0x168);
            if (t != 0x1000) {
                s32 d = 0xaaa - sp18;
                sp18 = (s16)(sp18 + (s16)(((long long)d * t + 0x800) >> 12));
            }
            if (data_0209f20c == 0 && data_0209f294 == 0 &&
                (u8)(data_0209f294 | (data_0209f2c4 | data_0209f20c)) != 0) {
                func_0200af20(self, &sp[0], &sp[3], &sp18);
            }
        }
    }

    sp[1] += *(s32 *)((char *)self + 0x134);
    sp[4] += *(s32 *)((char *)self + 0x134);

    s16 tr3 = *(s16 *)((char *)self + 0x18c);
    if (tr3 != 0) {
        s16 c = data_02082214[((s32)*(u16 *)((char *)self + 0x18a) >> 4) * 2];
        var_r4 = (s16)(var_r4 + (s16)(((long long)tr3 * c + 0x800) >> 12));
    }

    s16 tr0 = *(s16 *)((char *)self + 0x18e);
    if (tr0 != 0) {
        s16 c = data_02082214[((s32)*(u16 *)((char *)self + 0x192) >> 4) * 2 + 1];
        s32 m = (int)(((long long)c * 0x360 + 0x800) >> 12);
        sp18 = (s16)(sp18 + ((tr0 * m) << 4 >> 16));
    }

    func_0200d954(self, sp18);

    *(volatile s32 *)0x04000580 =
        *(u8 *)((char *)self + 0x10c) |
        (*(u8 *)((char *)self + 0x10d) << 8) |
        (*(u8 *)((char *)self + 0x10e) << 16) |
        (*(u8 *)((char *)self + 0x10f) << 24);

    s32 var_r6;
    if (data_0209f2f8 == 0x28) {
        var_r6 = 0x10;
    } else {
        var_r6 = data_0209ee90[0x44 / 4];
    }

    s32 idx = ((s32)(u16)sp18 >> 4) * 2;
    G3i::PerspectiveW_(
        data_02082214[idx],
        data_02082214[idx + 1],
        *(s32 *)((char *)self + 0xf8),
        *(s32 *)((char *)self + 0xfc),
        *(s32 *)((char *)self + 0x100),
        var_r6, 1, (Matrix4x3 *)0);

    {
        s32 s2 = (s32)(sp[2] + 4) >> 3;
        s32 s1 = (s32)(sp[1] + 4) >> 3;
        s32 s0 = (s32)(sp[0] + 4) >> 3;
        sp[0] = s0;
        sp[1] = s1;
        sp[2] = s2;
        sp[5] = (s32)(sp[5] + 4) >> 3;
        s32 s4 = (s32)(sp[4] + 4) >> 3;
        sp[3] = (s32)(sp[3] + 4) >> 3;
        sp[4] = s4;
    }

    void *lookdir = &data_02086efc;
    if (Vec3_HorzDist((Vector3 *)&sp[0], (Vector3 *)&sp[3]) == 0) {
        lookdir = &data_02086f08;
    }
    G3i::LookAt_((Vector3 *)&sp[3], (Vector3 *)lookdir, (Vector3 *)&sp[0], 1, (Matrix4x3 *)((char *)self + 0x50));

    if (var_r4 != 0) {
        Matrix4x3_FromRotationZ(&data_020a0e68, var_r4);
        MulMat4x3Mat4x3((Matrix4x3 *)((char *)self + 0x50), &data_020a0e68, (Matrix4x3 *)((char *)self + 0x50));
    }

    View::Render();
    return 1;
}
