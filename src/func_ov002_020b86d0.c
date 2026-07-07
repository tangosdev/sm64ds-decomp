typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;
typedef signed char s8;

extern void _ZN9Animation8LoadFileER13SharedFilePtr(void *sfp);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *thiz, void *bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void *thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *thiz, void *actor, s32 f1, s32 f2, u32 a, u32 b);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *thiz, void *actor, s32 f1, s32 f2, void *v, void *w);
extern int _ZN5Actor13ClosestPlayerEv(void *thiz);
extern void func_ov002_020b7f2c(void *c, void *p);
extern void func_ov002_020b7f7c(void *thiz);
extern void func_ov001_020ab228(void *c, void *a1, int idx, int a3, int a5);

extern char data_ov002_0210de50;
extern char data_ov002_0210de60;
extern char data_ov002_0210de48;
extern char data_ov002_0210de28;
extern char data_ov002_0210de08;
extern char data_ov002_0210de20;
extern char data_ov002_0210de40;
extern char data_ov002_0210de10;
extern char data_ov002_0210de00;
extern char data_ov002_0210de58;
extern char data_ov002_0210de18;
extern char data_ov002_0210de30;
extern char data_ov002_0210de38;
extern void *data_ov002_020ff0ac[];
extern char data_ov002_0210df64;
extern char data_ov002_0210df84;
extern char data_ov002_0210df04;
extern char data_ov002_0210df24;
extern char data_ov002_0210df54;
extern char data_ov002_0210df74;
extern char data_ov002_0210df14;
extern char data_ov002_0210df44;
extern char data_ov002_0210df34;

int func_ov002_020b86d0(char *c)
{
    int flag;
    int v;

    *(s32 *)(c + 0x3f0) = *(u32 *)(c + 8) & 0xff;
    *(s32 *)(c + 0x3f4) = (*(u32 *)(c + 8) >> 8) & 0xf;
    *(u8 *)(c + 0x400) = (*(u32 *)(c + 8) >> 0xc) & 0xf;

    if (*(s32 *)(c + 0x3f0) == 0xff)
        *(s32 *)(c + 0x3f0) = 0;

    if (*(s32 *)(c + 0x3f4) >= 3)
        return 0;

    if (*(s32 *)(c + 0x3f0) == 0x11 || *(s32 *)(c + 0x3f0) == 4) {
        if (*(u8 *)(c + 0x400) > 2)
            *(u8 *)(c + 0x400) = 0;
    } else {
        if (*(u8 *)(c + 0x400) > 1)
            *(u8 *)(c + 0x400) = 0;
    }

    switch (*(s32 *)(c + 0x3f0)) {
    case 0xf:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de50);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de60);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de48);
        break;
    case 0x14:
    case 0x15:
    case 0x16:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de28);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de08);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de20);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de40);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de10);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de00);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de58);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de18);
        break;
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    default:
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de30);
        _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov002_0210de38);
        break;
    }

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300,
            _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_020ff0ac[*(s32 *)(c + 0x3f4)]),
            1, -1) == 0)
        return 0;

    _ZN11ShadowModel12InitCylinderEv(c + 0x364);

    *(s32 *)(c + 0x80) = 0x1000;
    *(s32 *)(c + 0x84) = 0x1000;
    *(s32 *)(c + 0x88) = 0x1000;

    flag = 0;

    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x1e000, 0x1e000, 0x800002, 0);
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x1e000, 0x16000, 0, 0);

    *(s32 *)(c + 0x3c4) = *(s32 *)(c + 0x5c);
    *(s32 *)(c + 0x3c8) = *(s32 *)(c + 0x60);
    *(s32 *)(c + 0x3cc) = *(s32 *)(c + 0x64);

    *(s32 *)(c + 0x9c) = -0x1000;
    *(s32 *)(c + 0xa0) = -0x1e000;

    switch (*(s32 *)(c + 0x3f0)) {
    default:
        break;
    case 0:
        *(s32 *)(((long long)(c + 0x12c)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
        *(u8 *)(c + 0x400) = 4;
        func_ov002_020b7f2c(c, &data_ov002_0210df64);
        break;
    case 1:
        *(u8 *)(c + 0x400) = 0xff;
        func_ov002_020b7f2c(c, &data_ov002_0210df84);
        break;
    case 2:
        *(s32 *)(((long long)(c + 0x12c)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x8000;
        *(u8 *)(c + 0x400) = 4;
        func_ov002_020b7f2c(c, &data_ov002_0210df04);
        break;
    case 3:
        *(u8 *)(c + 0x400) = 0xff;
        func_ov002_020b7f2c(c, &data_ov002_0210df24);
        break;
    case 19:
        *(u8 *)(c + 0x400) = 0xff;
        *(s32 *)(c + 0x3c0) = _ZN5Actor13ClosestPlayerEv(c);
        func_ov002_020b7f2c(c, &data_ov002_0210df54);
        *(u8 *)(c + 0x401) = 1;
        break;
    case 20:
    case 21:
    case 22:
        func_ov002_020b7f7c(c);
        /* fallthrough */
    case 10:
    case 15:
        *(u8 *)(c + 0x400) = 0xff;
        func_ov002_020b7f2c(c, &data_ov002_0210df74);
        break;
    case 12:
        *(u8 *)(c + 0x400) = 4;
        func_ov002_020b7f2c(c, &data_ov002_0210df14);
        break;
    case 13:
        *(u8 *)(c + 0x400) = 4;
        func_ov002_020b7f2c(c, &data_ov002_0210df44);
        break;
    case 14:
        *(u8 *)(c + 0x400) = 2;
        *(s32 *)(c + 0x114) = 0x32000;
        *(s32 *)(c + 0x118) = 0x32000;
        *(u32 *)(c + 8) -= 0xa;
        *(s32 *)(c + 0x3f0) = 4;
        func_ov002_020b7f2c(c, &data_ov002_0210df34);
        break;
    case 17:
        *(s32 *)(c + 0x114) = 0x32000;
        *(s32 *)(c + 0x118) = 0x32000;
        flag = 1;
        func_ov002_020b7f2c(c, &data_ov002_0210df34);
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        *(u8 *)(c + 0x400) = 0xff;
        *(s32 *)(c + 0x114) = 0x32000;
        *(s32 *)(c + 0x118) = 0x32000;
        func_ov002_020b7f2c(c, &data_ov002_0210df34);
        break;
    case 5:
    case 11:
    case 18:
        *(s8 *)(c + 0xcc) = -1;
        /* fallthrough */
    case 16:
        *(u8 *)(c + 0x400) = 3;
        /* fallthrough */
    case 4:
        *(s32 *)(c + 0x114) = 0x32000;
        *(s32 *)(c + 0x118) = 0x32000;
        func_ov002_020b7f2c(c, &data_ov002_0210df34);
        break;
    }

    *(s32 *)(c + 0x35c) = 0x1000;

    if (*(u8 *)(c + 0x400) != 0xff) {
        if (flag != 0)
            v = 1;
        else
            v = 0;
        func_ov001_020ab228(c + 0x3d0, c, *(s32 *)(c + 0x3f4) & 0xff, *(u8 *)(c + 0x400), v);
    }

    *(u32 *)(c + 8) &= 0xfff;
    return 1;
}
