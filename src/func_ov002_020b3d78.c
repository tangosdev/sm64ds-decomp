typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
enum { false = 0, true = 1 };

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* bmd, int a, int b);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern int _ZN5Actor9TrackStarEjj(void* self, unsigned int a, unsigned int b);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, int f, s16 s, void* clps);
extern int _ZN5Event6GetBitEj(unsigned int b);
extern void* _ZN5Actor13ClosestPlayerEv(void* self);
extern int _ZN6Player15IsEnteringLevelEv(void* self);
extern int func_ov002_020b36a0(char* c);
extern int IsStarCollectedInCurLevel(int a);

extern char* data_ov002_02108ab0[];
extern char* data_ov002_02108ab4[];
extern char* data_ov002_02108ab8[];
extern signed char LEVEL_ID;
extern u8 STAR_ID;

int func_ov002_020b3d78(void* self)
{
    u8* c = (u8*)self;
    u16 id = *(u16*)(c+0xc);
    u8 idx;

    switch (id) {
    case 0xf:
    case 0x10:
    case 0x13: *(u8*)(c+0x32c) = 0; break;
    case 0x11: *(u8*)(c+0x32c) = 1; break;
    case 0x12: break;
    case 0x2e: *(u8*)(c+0x32c) = 2; break;
    }

    {
        void* f = _ZN5Model8LoadFileER13SharedFilePtr(
            *(void**)((char*)data_ov002_02108ab0 + *(u8*)(c+0x32c) * 0xc));
        _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);

    {
    int t11 = *(u16*)(c+0xc);
    t11 = t11 == 0x11;
    if (t11 != false) {
        int byte1;
        *(u8*)(c+0x322) = *(s32*)(c+8);
        byte1 = ((unsigned int)*(s32*)(c+8) >> 8) & 0xff;
        if (*(u8*)(c+0x322) == 0xff)
            *(u8*)(c+0x322) = 0;
        if (*(u8*)(c+0x322) != 0 && byte1 == 0xff) {
            *(u8*)(c+0x323) = _ZN5Actor9TrackStarEjj(c, *(u8*)(c+0x322), 2);
        }
    }
    }

    idx = *(u8*)(c+0x32c);
    if (idx <= 1) {
        int t10 = *(u16*)(c+0xc);
        t10 = t10 == 0x10;
        if (t10 != false) {
            void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(
                *(void**)((char*)data_ov002_02108ab4 + idx * 0xc));
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                c+0x124, kcl, c+0x2ec, 0x1800, *(s16*)(c+0x8e),
                *(void**)((char*)data_ov002_02108ab8 + idx * 0xc));
        } else {
            void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(
                *(void**)((char*)data_ov002_02108ab4 + idx * 0xc));
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                c+0x124, kcl, c+0x2ec, 0x1000, *(s16*)(c+0x8e),
                *(void**)((char*)data_ov002_02108ab8 + idx * 0xc));
        }
    } else {
        void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(
            *(void**)((char*)data_ov002_02108ab4 + idx * 0xc));
        _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
            c+0x124, kcl, c+0x2ec, 0x199, *(s16*)(c+0x8e),
            *(void**)((char*)data_ov002_02108ab8 + idx * 0xc));
    }

    {
    int t13 = *(u16*)(c+0xc);
    t13 = t13 == 0x13;
    if (t13 != false)
        *(u8*)(c+0x320) = *(s32*)(c+8) & 0xf;
    }
    *(u8*)(c+0x31f) = _ZN5Event6GetBitEj(*(u8*)(c+0x320));

    {
    void* pl = _ZN5Actor13ClosestPlayerEv(c);
    if (pl != 0 && _ZN6Player15IsEnteringLevelEv(pl) != 0) {
        int t11b = *(u16*)(c+0xc);
        t11b = t11b == 0x11;
        if (t11b != false && func_ov002_020b36a0((char*)c) != 0) {
            if (LEVEL_ID == 1)
                return 0;
        }
    }
    }

    {
    int t2e = *(u16*)(c+0xc);
    t2e = t2e == 0x2e;
    if (t2e != false && LEVEL_ID == 7) {
        if (STAR_ID != 1) {
            if (IsStarCollectedInCurLevel(1) != 0)
                goto ret1;
        }
        if (*(s32*)(c+0x60) >= 0xdac000)
            return 0;
    }
    }

ret1:
    return 1;
}
