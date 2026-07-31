//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern "C" void LoadSilverStarAndNumber(void);
extern "C" int _ZN5Model8LoadFileER13SharedFilePtr(int p);
extern "C" void _ZN5Event8ClearBitEj(unsigned int b);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, int f, int a, int b);
extern "C" void _ZN8Platform21UpdateModelPosAndRotYEv(void *self);
extern "C" void func_ov002_020b9f80(char *self);
extern "C" int _ZN12MeshCollider8LoadFileER13SharedFilePtr(int p);
extern "C" void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, int f, void *mtx, int fix, s16 s, int clps);
extern "C" void func_020393c4(void *p, void *v);

extern "C" int data_ov002_0211092c;
extern "C" char data_ov002_021098e8[];
extern "C" char data_ov002_021098ec[];
extern "C" char data_ov002_021098f0[];
extern "C" int func_ov002_020baa98;

extern "C" int _ZN10StarSwitch13InitResourcesEv(char *c) {
    u8 idx;
    int f;

    *(int*)(c + 0x320) = 0x1000;
    *(int*)(c + 0x324) = 0x1000;
    *(int*)(c + 0x328) = 0x1000;
    *(int*)(c + 0x344) = 0;
    *(int*)(c + 0x348) = 0;
    *(u8*)(c + 0x353) = *(signed char*)(c + 0xcc);

    {
        int b = 0;
        if (*(u16*)(c + 0xc) == 0xc) b = 1;
        if (b) {
            *(int*)(((long long)(int)(c + 0xb0))) |= 0x4000000;
            *(int*)(c + 0x33c) = 2;
            *(u8*)(c + 0x351) = *(unsigned int*)(c + 8);
            if (*(u8*)(c + 0x351) == 0xff)
                *(u8*)(c + 0x351) = 0;
            *(u16*)(c + 0x33a) = (*(unsigned int*)(c + 8) >> 8) & 0xff;
            LoadSilverStarAndNumber();
            _ZN5Model8LoadFileER13SharedFilePtr((int)&data_ov002_0211092c);
            *(u8*)(c + 0x34c) = 1;
        } else {
            *(int*)(c + 0x33c) = *(unsigned int*)(c + 8) & 3;
            *(u8*)(c + 0x34e) = (*(unsigned int*)(c + 8) >> 3) & 0xf;
            *(u16*)(c + 0x33a) = (*(unsigned int*)(c + 8) >> 8) & 0xff;
            _ZN5Event8ClearBitEj(*(u8*)(c + 0x34e));
            *(u8*)(c + 0x34c) = 0;
        }
    }

    idx = *(u8*)(c + 0x34c);
    f = _ZN5Model8LoadFileER13SharedFilePtr(*(int*)(data_ov002_021098e8 + idx * 0xc));
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0xd4, f, 1, -1);
    _ZN8Platform21UpdateModelPosAndRotYEv(c);
    func_ov002_020b9f80(c);

    idx = *(u8*)(c + 0x34c);
    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(*(int*)(data_ov002_021098ec + idx * 0xc));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, f, c + 0x2ec, 0x199, *(s16*)(c + 0x8e), *(int*)(data_ov002_021098f0 + idx * 0xc));

    func_020393c4(c + 0x124, &func_ov002_020baa98);

    {
        u16 h = *(u16*)(c + 0x300 + 0x3a);
        if (h == 0xff || h == 0)
            *(u16*)(c + 0x300 + 0x3a) = 0x190;
        else
            *(u16*)(((long long)(int)(c + 0x33a))) *= 0xa;
    }
    *(u8*)(c + 0x34f) = 5;
    *(int*)(((long long)(int)(c + 0x60))) += 0x5000;
    *(u8*)(c + 0x34d) = 1;
    return 1;
}
