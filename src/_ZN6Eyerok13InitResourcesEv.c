typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

typedef struct { s32 x, y, z; } Vector3;

extern int data_ov066_0211ae6c[];
extern int data_ov066_0211ae4c[];
extern int data_ov066_0211aeb4[];
extern int data_ov066_0211aebc[];
extern int data_ov066_0211ae9c[];
extern int data_ov066_0211ae3c[];
extern int data_ov066_0211ae2c[];
extern int data_ov066_0211ae5c[];
extern int data_ov066_0211ae84[];
extern int data_ov066_0211aea4[];
extern int data_ov066_0211ae8c[];
extern int data_ov066_0211ae54[];
extern int data_ov066_0211ae94[];
extern int data_ov066_0211ae64[];
extern int data_ov066_0211ae44[];
extern int data_ov066_0211ae74[];
extern int data_ov066_0211ae7c[];
extern int data_ov066_0211ae24[];
extern int data_ov066_0211aeac[];
extern int data_ov066_0211ae14[];
extern int data_ov066_0211ae1c[];
extern int data_ov066_0211ae34[];
extern int data_ov066_0211ad18[];
extern s8 data_ov066_0211abe0;
extern s32 data_ov066_0211abe4;
extern s8 data_ov066_0211ae00;
extern s8 data_ov066_0211ae04;
extern s8 data_ov066_0211ae08;
extern s8 data_ov066_0211ae0c;
extern s8 data_ov066_0211ae10;
extern int data_ov066_0211b05c[];
extern int data_ov066_0211b09c[];
extern char _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_;
extern char func_02112c08;
extern char func_02112ca8;
extern char func_02112d48;
extern char func_ov066_0211a35c;

extern u8 _ZN5Actor9TrackStarEjj(void* actor, u32 a, u32 b);
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* sfp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, void* bmd, int a, int b);
extern void _ZN15TextureSequence8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN9Animation8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* sfp);
extern void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(int bmd, int btp);
extern void _ZN11ShadowModel12InitCylinderEv(void* thiz);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* thiz, void* actor, Vector3* v, s32 f1, s32 f2, u32 a, u32 b);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 id, u32 b, Vector3* pos, void* p, int e, int f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, int kcl, void* mtx, s32 fix, s16 s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* thiz, void* actor);
extern void func_ov066_02119454(void* c, void* p);
extern void func_020398fc(void* p);

int _ZN6Eyerok13InitResourcesEv(void* arg0)
{
    char* c = (char*)arg0;
    Vector3 v;
    Vector3 w;

    *(s32*)(c + 0x49C) = *(s32*)(c + 8) & 0xFF;
    if (*(s32*)(c + 0x49C) == 0xFF)
        *(s32*)(c + 0x49C) = 0;
    *(u8*)(c + 0x672) = (*(u32*)(c + 8) >> 0xC) & 0xF;
    *(u8*)(c + 0x673) = _ZN5Actor9TrackStarEjj(c, *(u8*)(c + 0x672), 2);
    if (*(s32*)(c + 0x49C) > 2)
        *(s32*)(c + 0x49C) = 0;

    switch (*(s32*)(c + 0x49C)) {
    case 0:
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x3D0, _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae6c), 1, -1);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211ae4c);
        _ZN5Model8LoadFileER13SharedFilePtr(data_ov066_0211aeb4);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211aebc);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae9c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae3c);
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(data_ov066_0211ae2c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae5c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae84);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211aea4);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae8c);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae54);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae94);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae64);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae44);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae74);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov066_0211ae7c);
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov066_0211ae24);
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov066_0211aeac);
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov066_0211ae14);
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov066_0211ae1c);
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov066_0211ae34);
        break;
    case 1:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x360, (void*)data_ov066_0211ae4c[1], 1, -1) == 0)
            return 0;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov066_0211ae4c[1], data_ov066_0211aebc[1]);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov066_0211ae4c[1], data_ov066_0211ae9c[1]);
        break;
    case 2:
        if (_ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x360, (void*)data_ov066_0211aeb4[1], 1, -1) == 0)
            return 0;
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov066_0211aeb4[1], data_ov066_0211ae3c[1]);
        _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(data_ov066_0211aeb4[1], data_ov066_0211ae2c[1]);
        break;
    }

    if (*(s32*)(c + 0x49C) != 0) {
        _ZN11ShadowModel12InitCylinderEv(c + 0x420);
        w.x = data_ov066_0211ad18[0];
        w.y = data_ov066_0211ad18[1];
        w.z = data_ov066_0211ad18[2];
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0x320, c, &w, 0x64000, 0x64000, 0x200002, 0);
    }

    {
        int i = 0;
        char* p = c;
        do {
            *(s32*)(p + 0x4DC) = 0;
            *(s32*)(p + 0x4E0) = 0;
            i += 1;
            *(s32*)(p + 0x4E4) = 0;
            p += 0xC;
        } while (i < 0x14);
    }

    *(s32*)(c + 0xA0) = -0x64000;
    *(s32*)(c + 0x86C) = 0;
    *(s32*)(c + 0x870) = 0;

    if (*(s32*)(c + 0x49C) == 0) {
        void* r;
        *(s32*)(((int)c + 0x64) & 0xFFFFFFFFFFFFFFFFLL) -= 0x7C000;
        *(s32*)(c + 0x4A4) = *(s32*)(c + 0x5C);
        *(s32*)(c + 0x4A8) = *(s32*)(c + 0x60);
        *(s32*)(c + 0x4AC) = *(s32*)(c + 0x64);
        v.x = *(s32*)(c + 0x5C);
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        v.x += 0x193000;
        r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xB0, 1, &v, 0, *(s8*)(c + 0xCC), -1);
        if (r != 0)
            *(s32*)(c + 0x86C) = *(s32*)((char*)r + 4);
        v.x = *(s32*)(c + 0x5C);
        v.x -= 0x18C000;
        r = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xB0, 2, &v, 0, *(s8*)(c + 0xCC), -1);
        if (r != 0)
            *(s32*)(c + 0x870) = *(s32*)((char*)r + 4);
        data_ov066_0211ae10 = 0;
        data_ov066_0211ae08 = 0;
        data_ov066_0211ae0c = 0;
        data_ov066_0211abe4 = 1;
        data_ov066_0211ae04 = 1;
        data_ov066_0211abe0 = 3;
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x674, data_ov066_0211ae24[1], c + 0x83C, 0x199, *(s16*)(c + 0x8E), &func_02112ca8);
        func_020393d4(c + 0x674, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
        func_020393c4(c + 0x674, &func_ov066_0211a35c);
        _ZN16MeshColliderBase6EnableEP5Actor(c + 0x674, c);
        *(s16*)(c + 0x4D2) = 0x64;
        func_ov066_02119454(c, data_ov066_0211b09c);
    } else {
        *(s32*)(c + 0x4A4) = *(s32*)(c + 0x5C);
        *(s32*)(c + 0x4A8) = *(s32*)(c + 0x60);
        *(s32*)(c + 0x4AC) = *(s32*)(c + 0x64);
        *(s32*)(c + 0x4B0) = *(s32*)(c + 0x5C);
        *(s32*)(c + 0x4B4) = *(s32*)(c + 0x60);
        *(s32*)(c + 0x4B8) = *(s32*)(c + 0x64);
        if (*(s32*)(c + 0x49C) == 1) {
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x674, data_ov066_0211ae14[1], c + 0x83C, 0x199, *(s16*)(c + 0x8E), &func_02112c08);
            *(s32*)(((int)c + 0x4A4) & 0xFFFFFFFFFFFFFFFFLL) -= 0x31F000;
        } else {
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(c + 0x674, data_ov066_0211aeac[1], c + 0x83C, 0x199, *(s16*)(c + 0x8E), &func_02112d48);
            *(s32*)(((int)c + 0x4A4) & 0xFFFFFFFFFFFFFFFFLL) += 0x31F000;
        }
        func_020393d4(c + 0x674, &_ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
        func_020393c4(c + 0x674, &func_ov066_0211a35c);
        func_020398fc(c + 0x674);
        *(s32*)(((int)c + 0x4AC) & 0xFFFFFFFFFFFFFFFFLL) -= 0x32000;
        *(s8*)(c + 0x4D8) = 3;
        data_ov066_0211ae00 = 0;
        func_ov066_02119454(c, data_ov066_0211b05c);
    }
    return 1;
}
