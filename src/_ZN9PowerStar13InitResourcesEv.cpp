//cpp
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

struct Vec3 { s32 x, y, z; };
struct SharedFilePtr { u32 id; void *ptr; };

extern "C" {
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern s8 data_0209f2f8;
extern s32 data_0209cef0;
extern s32 data_02092138;
extern struct Vec3 data_ov002_0210aa0c;
extern struct SharedFilePtr data_ov002_02110924;
extern struct SharedFilePtr data_ov002_02110934;
extern struct SharedFilePtr data_ov002_02110944;
extern struct SharedFilePtr data_ov002_0211094c;
extern struct SharedFilePtr data_ov002_02110954;
extern struct SharedFilePtr data_ov002_0211095c;
extern struct SharedFilePtr data_ov002_02110964;

extern void _ZN9Animation8LoadFileER13SharedFilePtr(struct SharedFilePtr *f);
extern s32 _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, s32 a, s32 b);
extern void LoadSilverStarAndNumber(void);
extern void func_ov002_020e6edc(void *self);
extern void func_ov002_020e6df8(void *self);
extern s32 SublevelToLevel(s32 sub);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, s32 a, s32 spd, u32 g);
extern s32 _ZN11ShadowModel12InitCylinderEv(void *self);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
    void *self, void *actor, struct Vec3 *v, s32 a, s32 b, u32 c, u32 d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
    void *self, void *actor, s32 a, s32 b, void *p1, void *p2);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void *self);
extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 id, u32 param, struct Vec3 *pos, void *rot, s32 a, s32 b);
extern s32 NumVsStarsObtained(void);
extern void func_ov002_020e9448(void *self);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void *self);
extern s32 IsStarCollectedInCurLevel(u32 idx);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern void func_ov002_020e8dd8(void *self);
extern void func_ov002_020e7d08(void *self);
extern void _ZN5Event8ClearBitEj(u32 bit);
extern char *_ZN5Actor10FindWithIDEj(u32 id);
extern void LinkSilverStarAndStarMarker(void *a, void *b);
extern s32 IsStarCollected(s32 level, s32 idx);
extern s32 _ZN5Actor18GetBitInDeathTableEv(void *self);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void *self);

extern s32 _ZN9PowerStar13InitResourcesEv(void *arg0);
}

#define U8(o) (*(u8 *)(t + (o)))
#define S8(o) (*(s8 *)(t + (o)))
#define U16(o) (*(u16 *)(t + (o)))
#define S32(o) (*(s32 *)(t + (o)))
#define LU32(o) (*(u32 *)((int)(((long long)(int)(t + (o))) & 0xFFFFFFFFFFFFFFFFLL)))
#define LU16(o) (*(u16 *)((int)(((long long)(int)(t + (o))) & 0xFFFFFFFFFFFFFFFFLL)))

s32 _ZN9PowerStar13InitResourcesEv(void *arg0)
{
    char *t = (char *)arg0;
    s32 ret;
    s32 b;
    u32 p;
    u32 k;
    s32 kind;
    char *sp;
    s32 *q;
    struct Vec3 v;
    struct Vec3 v2;

    ret = 1;
    U16(0x4a2) = 0;
    U8(0x49c) = 0;
    U16(0x496) = 0xffff;
    U8(0x499) = (u8)S8(0xcc);
    S8(0x498) = -1;
    S32(0x80) = 0x1000;
    S32(0x84) = 0x1000;
    S32(0x88) = 0x1000;
    S32(0x434) = 0;
    S32(0x430) = 0;
    S32(0x4c0) = 0;
    S32(0x4bc) = S32(0x4c0);
    S32(0x4b8) = S32(0x4bc);
    S32(0x4b4) = S32(0x4b8);
    U16(0x492) = 0;
    U16(0x490) = 0;
    U8(0x49e) = 0xff;
    S32(0x478) = S32(0x5c);
    S32(0x47c) = S32(0x60);
    S32(0x480) = S32(0x64);
    U8(0x4a1) = 0;
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110944);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110924);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110964);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_02110934);

    b = (s32)(*(u16 *)(t + 0xc) == 0xb2);
    if (b != 0) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x30c, data_ov002_0211094c.ptr, 1, 1) == 0 ||
            _ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x370, data_ov002_0211095c.ptr, 1, 0x18) == 0)
            ret = 0;
    } else {
        LoadSilverStarAndNumber();
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x30c, data_ov002_02110954.ptr, 1, 1) == 0 ||
            _ZN9ModelBase7SetFileEP8BMD_Fileii(t + 0x370, data_ov002_02110954.ptr, 1, 1) == 0)
            ret = 0;
    }

    p = *(u32 *)(t + 8);
    k = p & 0x7f;
    if (k == 0x7f) {
        func_ov002_020e6edc(t);
        return ret;
    }
    if (k == 0x6f) {
        func_ov002_020e6df8(t);
        return ret;
    }
    S32(0x43c) = (s32)((p >> 4) & 0xf);

    b = (s32)(*(u16 *)(t + 0xc) == 0xb2);
    if (b != 0) {
        if (data_0209f220 == (*(u32 *)(t + 8) & 0xf) || SublevelToLevel(data_0209f2f8) > 0xe)
            U8(0x49a) = 2;
        else
            U8(0x49a) = 0;
        if (S32(0x43c) == 6) {
            LoadSilverStarAndNumber();
            LU32(0xb0) |= 0x4000000;
        }
    } else {
        U8(0x49a) = 1;
    }

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(t + 0x30c, data_ov002_02110964.ptr, 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(t + 0x370, data_ov002_02110964.ptr, 0x40000000, 0x1000, 0);
    if (_ZN11ShadowModel12InitCylinderEv(t + 0x3d4) == 0)
        return 0;

    v2.x = data_ov002_0210aa0c.x;
    v2.y = data_ov002_0210aa0c.y;
    v2.z = data_ov002_0210aa0c.z;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        t + 0x110, t, &v2, 0x64000, 0x96000, 0x100002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(t + 0x150, t, 0x50000, 0, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(t + 0x150);
    U8(0x49d) = (u8)(*(u32 *)(t + 8) & 0xf);
    if (S32(0x43c) != 7 && S32(0x43c) != 3)
        LU16(0x4a2) |= 2;

    kind = S32(0x43c);
    if (kind == 0 || (u32)(kind - 5) <= 2) {
        S32(0x440) = 4;
        if (S32(0x43c) != 6) {
            b = (s32)(*(u16 *)(t + 0xc) == 0xb3);
            if (b != 0) {
                S32(0x9c) = -0x2000;
                S32(0xa0) = -0x28000;
            }
        } else {
            v.x = S32(0x5c);
            v.y = S32(0x60);
            v.z = S32(0x64);
            v.y = v.y + 0xa000;
            sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb4, 0x40, &v, 0, S8(0x499), -1);
            if (sp != 0) {
                S32(0x434) = *(s32 *)(sp + 4);
            } else {
                return 0;
            }
            if (NumVsStarsObtained() == 5) {
                S32(0x80) = 0x1000;
                S32(0x84) = 0x1000;
                S32(0x88) = 0x1000;
            } else {
                S32(0x80) = 0;
                S32(0x84) = 0;
                S32(0x88) = 0;
            }
        }
        if (S32(0x43c) != 0)
            LU32(0x128) |= 1;
    } else if (kind == 1) {
        S32(0x440) = 8;
        S32(0xa8) = 0x20000;
        func_ov002_020e9448(t);
        U16(0x100) = 0xf;
        U16(0x494) = 0x32;
        _ZN12WithMeshClsn19StartDetectingWaterEv(t + 0x150);
    } else if (kind == 2 || kind == 4) {
        S32(0x440) = 0;
        LU32(0x128) |= 1;
    } else {
        S32(0x440) = 9;
        LU16(0x4a2) |= 8;
        LU32(0x128) |= 1;
        if (U8(0x49a) != 1)
            U8(0x49a) = 2;
    }

    S32(0x444) = S32(0x440);
    S32(0x448) = S32(0x5c);
    S32(0x44c) = S32(0x60);
    S32(0x450) = S32(0x64);
    q = (s32 *)((int)(((long long)(int)(t + 0x448)) & 0xFFFFFFFFFFFFFFFFLL));
    S32(0x454) = q[0];
    S32(0x458) = q[1];
    S32(0x45c) = q[2];
    S32(0x484) = data_02092138;
    if ((((u32)(U16(0x4a2) << 30)) >> 31) == 0)
        LU32(0x128) |= 1;

    if (U8(0x49d) < 8 && S32(0x43c) != 5 && S32(0x43c) != 3 &&
        (s32)(data_0209f2d8 == 1) == 0 && IsStarCollectedInCurLevel(U8(0x49d)) != 0) {
        if (SublevelToLevel(data_0209f2f8) == 0x1d) {
            _ZN9ActorBase18MarkForDestructionEv(t);
            return 0;
        }
        LU16(0x4a2) |= 4;
    }

    if (S32(0x43c) == 0 || S32(0x43c) == 5 || S32(0x43c) == 7 || S32(0x43c) == 1)
        func_ov002_020e8dd8(t);
    func_ov002_020e7d08(t);
    if (data_0209cef0 == 0) {
        _ZN5Event8ClearBitEj(0x1e);
        _ZN5Event8ClearBitEj(0x1d);
        if (S32(0x43c) != 3) {
            if ((s32)(data_0209f2d8 == 1) != 0 || (s32)(*(u16 *)(t + 0xc) == 0xb3) != 0) {
                sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0xb4, 0x50, (struct Vec3 *)(t + 0x5c), 0, S8(0x499), -1);
                if (sp != 0) {
                    S32(0x434) = *(s32 *)(sp + 4);
                } else {
                    return 0;
                }
            }
        }
    }

    sp = _ZN5Actor10FindWithIDEj((u32)S32(0x434));
    if (sp != 0)
        LinkSilverStarAndStarMarker(sp, t);
    if (data_0209f2f8 == 8 && U8(0x49d) == 7) {
        if (IsStarCollected(SublevelToLevel(8), 1) == 0 || data_0209f220 == 1) {
            _ZN9ActorBase18MarkForDestructionEv(t);
            return ret;
        }
    }
    if (data_0209f2f8 == 7 && U8(0x49d) == 2) {
        if (data_0209f220 == 1 || IsStarCollectedInCurLevel(1) == 0) {
            _ZN9ActorBase18MarkForDestructionEv(t);
            return 0;
        }
    }
    if (_ZN5Actor18GetBitInDeathTableEv(t) != 0 && U8(0x49d) == 1 && data_0209f2f8 == 0x2e) {
        _ZN5Actor24KillAndTrackInDeathTableEv(t);
        sp = _ZN5Actor10FindWithIDEj((u32)S32(0x434));
        if (sp != 0)
            LinkSilverStarAndStarMarker(sp, 0);
    }
    return ret;
}
