//cpp
// NONMATCHING: constant / value (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef signed char s8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;
struct BCA_File;

extern u16 RNG_STATE;
extern void* data_ov073_021232a0[];
extern char data_ov073_02123360;
extern char data_ov073_021233a0;
extern char data_ov073_021233d0;

extern "C" {
extern u16 DecIfAbove0_Short(void* p);
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_ov073_0211f494(void* c, void* p);
extern int _ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(void* c, void* clsn, void* player);
extern void func_02012694(int a, void* b);
extern int RandomIntInternal(u16* seed);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const Vector3* v, void* rot, s32 e, s32 f);
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 id);
extern void func_ov073_0212157c(void* c, void* p);
extern int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12 f, u32 b, u32 c, u32 d);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12 x, Fix12 y, Fix12 z);
extern void _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(void* thiz, struct BCA_File* f, int i, int j, Fix12 fx, u16 k);
}

extern "C" s32 func_ov073_0211f61c(char* c)
{
    void* r5;
    s32 r4;
    u32 id;

    if (DecIfAbove0_Short(c + 0x4cc) != 0)
        return 0;
    id = *(u32*)(c + 0x134);
    if (id == 0)
        return 0;
    r5 = _ZN5Actor10FindWithIDEj(id);
    if (!r5)
        return 0;

    r4 = 0;
    if (*(s32*)(c + 0x130) & 0x6000) {
        func_ov073_0211f494(c, c);
        *(s32*)(c + 0x98) = 0x20000;
        r4 = 1;
    }

    {
        int isBf = (int)(*(u16*)((char*)r5 + 0xc) == 0xbf);
        if (isBf == 1) {
            if (*(u8*)((char*)r5 + 0x703) != 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x107e0;
                if (flags) {
                    *(s32*)(c + 0x98) = 0x41000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                s32 flags = *(s32*)(c + 0x130) & 0x50380;
                if (flags) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x2d000;
                    r4 = 1;
                }
            }
            if (r4 == 0) {
                if ((*(s32*)(c + 0x130) & 0x70) || (*(u8*)((char*)r5 + 0x6f9) != 0)
                    || (_ZN5Actor16JumpedOnByPlayerER12CylinderClsnR6Player(c, c + 0x110, r5) != 0)) {
                    func_ov073_0211f494(c, c);
                    *(s32*)(c + 0x98) = 0x20000;
                    r4 = 1;
                }
            }
        }
    }

    if (r4 == 0) {
        if (*(s32*)(c + 0x130) & 0x400) {
            func_ov073_0211f494(c, c);
            *(s32*)(c + 0x98) = 0x3d000;
            r4 = 1;
        }
    }

    if (r4 != 0) {
        s32 count;
        s32 i;
        s32 shortY;
        Vector3 v;
        s32 rnd;

        *(u16*)(c + 0x4cc) = 0x10;
        func_02012694(0x16b, c + 0x74);
        rnd = RandomIntInternal(&RNG_STATE);
        if (((rnd >> 8) & 7) == 0) {
            v.x = *(s32*)(c + 0x5c);
            v.y = *(s32*)(c + 0x60);
            v.z = *(s32*)(c + 0x64);
            count = (rnd >> 0xc) & 3;
            v.y = v.y + 0x64000;
            if (count == 0)
                count = 1;
            i = 0;
            if (count > 0) {
                do {
                    void* actor = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x120, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                    if (actor != 0) {
                        rnd = RandomIntInternal(&RNG_STATE);
                        shortY = ((s32)(((((u32)rnd >> 8) & 0xf) << 0x1c))) >> 0x10;
                        *(s16*)((char*)actor + 0x92) = 0;
                        *(s16*)((char*)actor + 0x94) = (s16)shortY;
                        *(s16*)((char*)actor + 0x96) = 0;
                        *(s32*)((char*)actor + 0x98) = 0xa000;
                        if (_ZN8SaveData19IsCharacterUnlockedEj(2) != 0) {
                            *(s32*)(c + 0x4c0) = *(s32*)(c + 0x4c0) + 1;
                            if (*(s32*)(c + 0x4c0) > 0x1e) {
                                void* actor2 = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x115, 0, &v, 0, *(s8*)(c + 0xcc), -1);
                                if (actor2 != 0) {
                                    *(s16*)((char*)actor2 + 0x92) = 0;
                                    *(s16*)((char*)actor2 + 0x94) = (s16)shortY;
                                    *(s16*)((char*)actor2 + 0x96) = 0;
                                    *(s32*)((char*)actor2 + 0x98) = 0xa000;
                                }
                                *(s32*)(c + 0x4c0) = 0;
                            }
                        }
                    }
                    i = i + 1;
                } while (i < count);
            }
        }
        {
            void* anim = *(void**)(c + 0x37c);
            if (anim != &data_ov073_021233d0 && anim != &data_ov073_021233a0) {
                func_ov073_0212157c(c, 0);
            }
        }
        return 1;
    }

    {
        Vector3 v2;
        v2.x = *(s32*)(c + 0x5c);
        v2.y = *(s32*)(c + 0x60);
        v2.z = *(s32*)(c + 0x64);
        if (_ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r5, &v2, 0, 0x14000, 1, 0, 1) == 0)
            goto done0;
    }
    {
        s32* pv = (s32*)(((u32)r5 + 0x5c) & 0xFFFFFFFFFFFFFFFF);
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8a, pv[0], pv[1], pv[2]);
    }
    if (*(void**)(c + 0x37c) == &data_ov073_021233d0) {
        func_02012694(0x16b, c + 0x74);
        return 1;
    }
    {
        s16* py = (s16*)(((u32)c + 0x94) & 0xFFFFFFFFFFFFFFFF);
        *py = (s16)(*py + 0x8000);
    }
    func_02012694(0x16a, c + 0x74);
    func_ov073_0212157c(c, &data_ov073_02123360);
    _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c + 0x30c, (struct BCA_File*)data_ov073_021232a0[1], 4, 0x40000000, 0x1000, 0);
    *(s32*)(c + 0x368) = 0x1000;
    *(u8*)(c + 0x4c8) = 2;
    *(s32*)(c + 0x4d0) = 0x2000;
    *(s16*)(c + 0x100) = 0x1e;
done0:
    return 0;
}
