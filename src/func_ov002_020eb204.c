typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;
typedef signed char s8;

struct Vec3 { s32 x, y, z; };
struct RaycastGround { char pad[0x50]; };

extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *actor, const void *v, int d, int e, u32 f, u32 g);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN4BgCh19StartDetectingWaterEv(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const void *v, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const void *v, const void *v16, int e, int f);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *f, int a, int b);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int SublevelToLevel(int i);
extern int IsStarCollectedInCurLevel(u8 x);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern int _ZN5Actor18GetBitInDeathTableEv(void *self);

extern char RED_NUMBER_MODEL_PTR;
extern char data_ov002_0211092c;
extern char data_ov002_0211093c;
extern u8 CURRENT_GAMEMODE;
extern s8 LEVEL_ID;

int func_ov002_020eb204(char *c)
{
    struct Vec3 pos;
    struct Vec3 v0;
    struct Vec3 v4;
    struct Vec3 v5;
    struct RaycastGround rg;
    u32 raw;
    u8 kind;
    int r3;

    raw = ((u32)*(s32 *)(c + 8) >> 4) & 0xf;
    kind = (u8)raw;
    *(u8 *)(c + 0x1db) = 0;
    v0.x = 0;
    v0.y = -0x50000;
    v0.z = 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0xd4, c, &v0, 0x50000, 0xa0000, 0x100002, 0x8000);
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN4BgCh19StartDetectingWaterEv(&rg);

    {
        s32 pyb = *(s32 *)(c + 0x60);
        s32 pz = *(s32 *)(c + 0x64);
        s32 px = *(s32 *)(c + 0x5c);
        s32 pyy = pyb + 0x1e000;
        pos.x = px;
        pos.y = pyy;
        pos.z = pz;
    }
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, &pos, c);
    if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0)
        *(s32 *)(c + 0x1c8) = *(s32 *)((char *)&rg + (0x80 - 0x3c));

    r3 = 0;
    *(u8 *)(c + 0x1d9) = (u8)(*(s32 *)(c + 8) & 0xf);
    *(u8 *)(c + 0x1d8) = 0;

    if (kind == 6) {
        void *sp;
        sp = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, *(u8 *)(c + 0x1d9) | 0x60, c + 0x5c, (void *)0, (s8)*(s8 *)(c + 0xcc), -1);
        if (sp != 0) {
            u16 *p = (u16 *)(((int)sp + 0x4a2) & 0xFFFFFFFFFFFFFFFFLL);
            *p = (u16)(*p | 0x80);
        }
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211093c), 1, 0x18);
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }

    if (kind == 4) {
        u8 *p;
        *(u8 *)(c + 0x1d8) = 2;
        p = (u8 *)(((int)c + 0x1db) & 0xFFFFFFFFFFFFFFFFLL);
        *p = (u8)(*p | 2);
        v4.x = 0;
        v4.y = -0x50000;
        v4.z = 0;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0xd4, c, &v4, 0x50000, 0xa0000, 0x100004, 0);
        *(u16 *)(c + 0x1d4) = 0;
    } else if (kind == 5) {
        *(u8 *)(c + 0x1d8) = 3;
        v5.x = 0;
        v5.y = -0x50000;
        v5.z = 0;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0xd4, c, &v5, 0x50000, 0xa0000, 1, 0);
    } else if (kind & 1) {
        *(u8 *)(c + 0x1d8) = 1;
        if (kind & 2) {
            u8 *p = (u8 *)(((int)c + 0x1db) & 0xFFFFFFFFFFFFFFFFLL);
            *p = (u8)((*p & ~1) | 1);
        }
        {
            u8 *p = (u8 *)(((int)c + 0x1db) & 0xFFFFFFFFFFFFFFFFLL);
            *p = (u8)(*p | 8);
        }
    } else {
        u8 *p = (u8 *)(((int)c + 0x1db) & 0xFFFFFFFFFFFFFFFFLL);
        u8 nv = (u8)((((int)kind >> 1) & 1) ^ 1);
        *p = (u8)((*p & ~2) | ((nv & 1) << 1));
    }

    if (*(u8 *)(c + 0x1d8) != 0) {
        _ZN5Model8LoadFileER13SharedFilePtr(&RED_NUMBER_MODEL_PTR);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211092c), 1, 0x19) == 0) {
            _ZN13RaycastGroundD1Ev(&rg);
            return 0;
        }
    } else {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0211093c), 1, 0x18) == 0) {
            _ZN13RaycastGroundD1Ev(&rg);
            return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x164) == 0) {
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }

    if (((u32)(*(u8 *)(c + 0x1db) << 0x1e) >> 0x1f) == 0) {
        s32 *p = (s32 *)(((int)c + 0xec) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p | 1;
    }
    r3 = 0;
    *(s32 *)(c + 0x1bc) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x1c0) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x1c4) = *(s32 *)(c + 0x64);
    *(s32 *)(c + 0x1cc) = r3;
    *(s16 *)(c + 0x1d6) = -1;
    *(s32 *)(c + 0x1d0) = r3;

    if (CURRENT_GAMEMODE == 1)
        r3 = 1;
    if (r3 == 0 && SublevelToLevel((s8)LEVEL_ID) == 0x1d && IsStarCollectedInCurLevel(*(u8 *)(c + 0x1d9)) != 0) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }
    if (_ZN5Actor18GetBitInDeathTableEv(c) != 0) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        _ZN13RaycastGroundD1Ev(&rg);
        return 0;
    }
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
