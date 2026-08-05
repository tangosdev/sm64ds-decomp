//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

typedef struct { s32 x, y, z; } Vector3;

struct Actor { Actor *ClosestPlayer(); };

struct ModelAnimV {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void UpdateVerts();
};

struct CylinderClsn {
    virtual ~CylinderClsn();
    void Clear();
    void Update();
};

extern "C" {
void func_ov085_02129570(void *c);
s32 Vec3_HorzDist(const void *a, const void *b);
s16 Vec3_HorzAngle(const void *a, const void *b);
s16 Vec3_VertAngle(const void *a, const void *b);
s32 AngleDiff(s32 a, s32 b);
void _Z14ApproachLinearRsss(s16 *dst, s16 target, s16 rate);
void _Z15ApproachLinear2Riii(s32 *dst, s32 target, s32 rate);
void *_ZN5Actor10FindWithIDEj(u32 id);
void _ZN9Animation7AdvanceEv(void *c);
void func_ov085_021295bc(void *c);
void _ZN9ModelBase12ApplyOpacityEj(void *c, u32 a, int b);
extern s8 data_0209f2f8;
}

extern "C" s32 _ZN4Toad8BehaviorEv(Actor *self)
{
    char *s = (char *)self;

    func_ov085_02129570(s);
    ((ModelAnimV *)(s + 0x108))->UpdateVerts();

    Actor *p = self->ClosestPlayer();
    if (p != 0) {
        s32 threshold = 0xfa000;
        if (data_0209f2f8 == 0x32)
            threshold = 0x1f4000;

        Vector3 *psrc = (Vector3 *)(((long long)(int)((char *)p + 0x5c)));
        Vector3 v;
        v.x = psrc->x;
        v.y = psrc->y;
        v.z = psrc->z;

        s32 hd = Vec3_HorzDist(s + 0x5c, &v);

        Vector3 v2;
        v2.y = v.y;
        v2.x = v.x;
        v2.z = v.z;
        v2.y = v.y + 0x1e000;

        s16 ha = Vec3_HorzAngle(s + 0x5c, &v2);
        s16 va = Vec3_VertAngle(s + 0x5c, &v2);

        if (hd < 0x190000)
            *(u8 *)(s + 0x20e) = 0xff;
        else
            *(u8 *)(s + 0x20e) = 0x3c;

        if (hd < threshold && AngleDiff(ha, *(s16 *)(s + 0x8e)) < 0x3000) {
            *(s16 *)(s + 0x202) = (s16)(ha - *(s16 *)(s + 0x8e));
            *(s16 *)(s + 0x206) = va;
        } else if (data_0209f2f8 != 0x32) {
            *(s16 *)(s + 0x202) = 0;
            *(s16 *)(s + 0x206) = 0;
        } else {
            _Z14ApproachLinearRsss((s16 *)(s + 0x8e), ha, 0x100);
            *(s16 *)(s + 0x202) = 0;
            *(s16 *)(s + 0x206) = 0;
        }
    } else {
        *(u8 *)(s + 0x20e) = 0x3c;
    }

    {
        u32 id = *(u32 *)(s + 0x1f4);
        if (id != 0) {
            if (_ZN5Actor10FindWithIDEj(id) != 0)
                *(u8 *)(s + 0x20e) = 0xff;
        }
    }

    if (data_0209f2f8 == 0x32)
        *(u8 *)(s + 0x20e) = 0xff;

    _Z14ApproachLinearRsss((s16 *)(s + 0x200), *(s16 *)(s + 0x202), 0x250);
    _Z14ApproachLinearRsss((s16 *)(s + 0x204), *(s16 *)(s + 0x206), 0x100);

    _Z15ApproachLinear2Riii((s32 *)(s + 0x20d), *(u8 *)(s + 0x20e), 6);

    _ZN9Animation7AdvanceEv(s + 0x158);

    func_ov085_021295bc(s);

    *(s32 *)(s + 0x164) = 0x1000;

    _ZN9ModelBase12ApplyOpacityEj(s + 0x108, (u32)(u8)(*(u8 *)(s + 0x20d) >> 3), 1);

    ((CylinderClsn *)(s + 0xd4))->Clear();
    ((CylinderClsn *)(s + 0xd4))->Update();

    return 1;
}
