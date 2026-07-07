#pragma opt_common_subs off

#define false 0
#define true 1

typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

typedef struct { void* sfp; void* bmd; } FileEntry;
typedef struct { u32 w[12]; } Blob48;

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern int SublevelToLevel(int i);
extern int _ZN5Actor18GetBitInDeathTableEv(void* actor);
extern void SetStarMarker(int i, void* actor, int v2);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 f1, s32 f2, void* v, s32 f3);
extern void _ZN12WithMeshClsn13SetLimMovFlagEv(void* thiz);
extern void _ZN12WithMeshClsn19StartDetectingWaterEv(void* thiz);

extern Blob48 data_02082128;
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern s32 data_0209f40c[];
extern u8 data_0209f2d8;
extern FileEntry* data_ov002_020ff06c[];
extern FileEntry* data_ov002_020ff060[];
extern void* data_ov002_0210d9a8;

int func_ov002_020b2508(char* c)
{
    s32 r5;
    s32 r4;
    s32 t;
    s8 i;
    s32 j;

    *(u8*)(c + 0x3ae) = 0;
    r5 = 0x64000;
    t = *(s32*)(c + 8) & 0xf;
    *(s32*)(c + 0x3a4) = t;
    r4 = 0x40000;
    t = *(s32*)(c + 0x3a4);
    if (t >= 9) {
        t = 1;
        *(s32*)(c + 0x3a4) = t;
    }
    t = *(s32*)(c + 0x3a4);
    if (t == 8) {
        *(s32*)(c + 0xa8) = 0x14000;
        *(s32*)(c + 0x9c) = -0x4000;
        *(s32*)(c + 0xa0) = -0x37000;
        *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) =
            (*(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) & ~0xe0) | 0x40;
        goto shared140;
    }
    if (t == 1 || t == 7) {
        goto case17;
    }
    if (t == 5) {
        goto case5;
    }
    /* default */
    *(s32*)(c + 0xa8) = 0x24000;
    *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
    goto block_11;
case5:
    {
        u32 flags = *(u32*)(c + 8);
        if (flags & 8) {
            *(s16*)(c + 0x94) = (s16)(((flags & 0x70) << 8) + 0x8000);
        } else {
            *(s16*)(c + 0x94) = (s16)((flags & 0x70) << 8);
        }
    }
block_11:
    *(s32*)(c + 0x9c) = -0x4000;
    *(s32*)(c + 0xa0) = -0x37000;
    *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xe0;
    goto shared140;
case17:
    *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) =
        (*(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) & ~0xe0) | 0xe0;
    *(s32*)(c + 0x398) = *(s32*)(c + 0x60) - 0x1f4000;
    if (*(s32*)(c + 0x3a4) == 7) {
        r5 = 0x32000;
        r4 = 0x28000;
    }
shared140:;

    *(Blob48*)(c + 0x368) = data_02082128;

    *(s8*)(c + 0x3ac) = -1;
    *(u8*)(c + 0x3ab) = 0xff;

    {
        u16 h;
        int b;
        h = *(u16*)(c + 0xc);
        b = h;
        b = (b == 0x121);
        if (b) {
            *(u8*)(c + 0x3ab) = (u8)((*(u32*)(c + 8) >> 4) & 7);
            _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9a8);
            *(s32*)(c + 0x3a0) = 1;
            if (SublevelToLevel(data_0209f2f8) == 0x13 ||
                *(u8*)(c + 0x3ab) == data_0209f220) {
                if (_ZN5Actor18GetBitInDeathTableEv(c) == 0) {
                    for (i = 0; i < 0xc; i = (s8)(i + 1)) {
                        if (data_0209f40c[i] == 0) {
                            SetStarMarker(i, c, 4);
                            *(s8*)(c + 0x3ac) = i;
                            break;
                        }
                    }
                }
            }
        } else {
            int b2;
            b2 = h;
            b2 = (b2 == 0x122);
            if (b2) {
                *(u8*)(c + 0x3ab) = (u8)((*(u32*)(c + 8) >> 4) & 7);
                *(s32*)(c + 0x3a0) = 2;
            } else {
                *(s32*)(c + 0x3a0) = 0;
            }
        }
    }

    j = *(s32*)(c + 0x3a0);
    if (j < 2) {
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd8, data_ov002_020ff06c[j]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, data_ov002_020ff060[*(s32*)(c + 0x3a0)]->bmd, 1, 1) == 0) {
            return 0;
        }
    } else {
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff06c[j]);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff060[*(s32*)(c + 0x3a0)]);
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd8, data_ov002_020ff06c[*(s32*)(c + 0x3a0)]->bmd, 1, 1) == 0) {
            return 0;
        }
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x114, data_ov002_020ff060[*(s32*)(c + 0x3a0)]->bmd, 1, 1) == 0) {
            return 0;
        }
    }

    if (_ZN11ShadowModel12InitCylinderEv(c + 0x150) == 0) {
        return 0;
    }

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x178, c, r5, r4, 0x100002, 0x8000);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x1ac, c, 0x3c000, 0x3c000, 0, 0);
    _ZN12WithMeshClsn13SetLimMovFlagEv(c + 0x1ac);
    _ZN12WithMeshClsn19StartDetectingWaterEv(c + 0x1ac);

    t = *(s32*)(c + 0x3a4);
    if (t == 8) {
        *(s16*)(c + 0x3a8) = 0x2d;
        *(s32*)(((int)c + 0x190) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
    } else {
        if (t == 6) {
            int b3;
            b3 = data_0209f2d8;
            b3 = (b3 == 1);
            if (b3 == false) {
                *(u16*)(c + 0x3a8) = 0xffffu;
                *(s32*)(((int)c + 0x190) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
                goto after438;
            }
        }
        *(s16*)(c + 0x3a8) = 0xd2;
    after438:;
    }

    *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) =
        (*(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) & ~1) | 1;

    t = *(s32*)(c + 0x3a4);
    if (t == 1 || t == 7) {
        goto case17b;
    }
    if (t == 5) {
        *(u8*)(c + 0x3aa) = 0;
    } else {
        *(u8*)(c + 0x3aa) = 0xf;
    }
    goto switch2end;
case17b:
    *(u8*)(c + 0x3aa) = 0;
    if (*(s32*)(c + 0x3a0) == 2 && (u32)*(u8*)(c + 0x3ab) < 8) {
        *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
        *(s32*)(((int)c + 0x190) & 0xFFFFFFFFFFFFFFFFLL) |= 1;
    }
switch2end:;

    *(u8*)(((int)c + 0x3ae) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x1c;
    *(s32*)(c + 0xd4) = 0;
    return 1;
}
