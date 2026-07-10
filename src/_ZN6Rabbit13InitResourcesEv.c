typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed char s8;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern char data_ov085_021305b8;
extern char data_ov085_021305d0;
extern char data_ov085_021305b0;
extern char data_ov085_021305c8;
extern char data_ov085_021305c0;
extern char data_ov085_021305d8;
extern char data_ov085_021305e0;
extern int data_0209caa0[];
extern s8 data_0209f2f8;
extern s32 data_ov085_021305ac;
extern int data_0209e650;
extern char data_ov085_021306cc;

extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern int _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* thiz, void* actor, s32 a, s32 b, u32 c, u32 d);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, s32 a, s32 b, void* v, void* w);
extern void _ZN7PathPtrC1Ev(void* thiz);
extern void _ZN7PathPtr6FromIDEj(void* thiz, u32 id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* thiz, void* v, u32 idx);
extern int _ZNK7PathPtr8NumNodesEv(void* thiz);
extern void func_ov085_0212bcc8(char* c);
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern int func_02013890(int r0, int r1);
extern u32 RandomIntInternal(int* seed);
extern u8 NumStars(void);
extern void func_ov085_0212bc78(void* c, void* p);

int _ZN6Rabbit13InitResourcesEv(char* c)
{
    char sp8[8];
    void* r0;
    void* r6;
    int r1;

    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305b8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305d0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305b0);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305c8);
    _ZN9Animation8LoadFileER13SharedFilePtr(&data_ov085_021305c0);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305d8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x300, _ZN5Model8LoadFileER13SharedFilePtr(&data_ov085_021305e0), 1, -1);
    _ZN11ShadowModel12InitCylinderEv(c + 0x368);
    _ZN11ShadowModel12InitCylinderEv(c + 0x3c0);

    *(s32*)(c + 0x438) = *(s32*)(c + 8) & 0xff;
    if (*(s32*)(c + 0x438) == 0xff)
        *(s32*)(c + 0x438) = 0;

    *(s32*)(c + 0x43c) = (*(s32*)(c + 8) & 0xf00) >> 8;
    if (*(s32*)(c + 0x43c) == 0xff)
        *(s32*)(c + 0x43c) = 0;

    *(s32*)(c + 0x440) = (*(s32*)(c + 8) & 0xf000) >> 0xc;
    if (*(s32*)(c + 0x440) == 0xf)
        *(s32*)(c + 0x440) = 0;

    r1 = *(s32*)(c + 0x43c);
    if (r1 != 7) {
        if (!(data_0209caa0[1] & 0x40000000))
            return 0;
    }

    if (r1 == 5 && *(s32*)(c + 0x440) == 0)
        goto check18;
    if (r1 == 1 && *(s32*)(c + 0x440) == 1)
        goto check18;
    if (r1 != 6)
        goto skip17;
    if (*(s32*)(c + 0x440) != 3)
        goto skip17;

check18:
    if (!(data_0209caa0[2] & 0x80000))
        return 0;

skip17:
    *(s32*)(c + 0x9c) = -0x1000;
    *(s32*)(c + 0xa0) = -0x1e000;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x110, c, 0x50000, 0x64000, 0xb00004, 0x9000);
    *(s32*)(c + 0x45c) = 0;
    *(s32*)(c + 0x35c) = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x144, c, 0x28000, 0x28000, 0, 0);
    _ZN7PathPtrC1Ev(sp8);
    _ZN7PathPtr6FromIDEj(sp8, *(s32*)(c + 0x438));
    *(s32*)(c + 0x448) = 1;
    _ZNK7PathPtr7GetNodeER7Vector3j(sp8, c + 0x5c, *(s32*)(c + 0x448));
    *(s32*)(c + 0x444) = _ZNK7PathPtr8NumNodesEv(sp8);
    func_ov085_0212bcc8(c);
    *(s32*)(c + 0xd0) = 0;
    *(s32*)(c + 0x80) = 0x1000;
    *(s32*)(c + 0x88) = *(s32*)(c + 0x80);
    *(s32*)(c + 0x84) = *(s32*)(c + 0x88);

    if (*(s32*)(c + 0x43c) == 7) {
        if (data_0209caa0[1] & 0x40)
            return 0;
        *(u8*)(c + 0x428) = 1;
        goto block_26;
    }

    r0 = _ZN5Actor13ClosestPlayerEv(c);
    if (r0 == 0)
        return 0;
    if (data_0209f2f8 != 0x32) {
        if (*(s32*)(c + 0x440) != *(u8*)((char*)r0 + 0x6d9))
            return 0;
    }

block_26:
    *(s32*)(c + 0x420) = *(s32*)(c + 0x440) + 1;
    if (*(s32*)(c + 0x440) == 3)
        *(s32*)(c + 0x420) = 0;

    if (*(s32*)(c + 0x43c) == 7) {
        *(s32*)(c + 0x420) = 0;
        goto block_out;
    }

    r6 = _ZN5Actor13ClosestPlayerEv(c);
    if (r6 == 0)
        goto block_out;

    {
        int v;
        v = *(s32*)((char*)r6 + 8);
        if (data_0209f2f8 == 0x32)
            v = 1;
        if (func_02013890(*(s32*)(c + 0x43c), v) != 0 && data_ov085_021305ac < 8) {
            u32 rnd = RandomIntInternal(&data_0209e650) >> 8;
            if (NumStars() >= 0x51) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0xf) == 0)
                        *(s32*)(c + 0x420) = 5;
                } else {
                    if ((rnd & 7) == 0)
                        *(s32*)(c + 0x420) = 5;
                }
            } else if (NumStars() >= 0x28) {
                if (*(s32*)((char*)r6 + 8) == 3) {
                    if ((rnd & 0x1f) == 0)
                        *(s32*)(c + 0x420) = 5;
                } else {
                    if ((rnd & 0xf) == 0)
                        *(s32*)(c + 0x420) = 5;
                }
            }
            if (*(s32*)(c + 0x420) == 5) {
                data_ov085_021305ac += 1;
                *(u8*)(c + 0x429) = 1;
            }
        }
    }

block_out:
    if (data_0209f2f8 == 5) {
        if (*(s8*)(c + 0xcc) == 3)
            *(s32*)(c + 0xb0) = 0x8280;
    }
    *(s32*)(c + 0x468) = (*(s32*)(c + 0x420) << 1) + *(s32*)(*(char**)(c + 0x30c) + 0x20);
    func_ov085_0212bc78(c, &data_ov085_021306cc);
    return 1;
}
