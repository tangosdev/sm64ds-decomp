//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

extern "C" {
    void Enable3dEngines(void);
    void func_0200f2cc(void);
    int func_0203d9b4(void);
    void LoadTextNarcs(void);
    int LoadArchive(int idx);
    void func_0200f220(void);
    int LoadFile(int handle);
    void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, u32 a, u32 b);
    void Deallocate(void* ptr);
    void SetBg0Offset(int a, int b);
    void SetBg2Offset(int a, int b);
    void SetBg3Offset(int a, int b);
    void SetSubBg0Offset(int a, int b);
    void SetSubBg2Offset(int a, int b);
    void SetSubBg3Offset(int a, int b);
    void LoadFont3D(void);
    int func_ov075_02116ad4(void* arg);
    int func_0203da3c(void);
    void UnloadArchive(int i);
    void func_02030aa4(int arg);
    int func_0203da9c(void);
    void func_02020028(int a, int b);
    void func_ov075_02116818(char* c);
    void func_ov075_0211601c(char* c, int a, int b);
    void _ZN5Sound16LoadInitialGroupEi(int a);
    void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int a);
}

extern u8 data_0209d45c;
extern u8 data_0209d454;
extern void* data_0209b2e8;
extern u8 data_020a0e40;
extern u8 data_0209f4ae[];
extern void* data_ov075_0211d71c;
extern void* data_0209d4a8;
extern u8 data_0209b2f0[];
extern s32 data_0208ee44;
extern u8 data_02092778;

extern "C" int func_ov075_0211a410(char* c)
{
    Enable3dEngines();
    func_0200f2cc();

    {
        int b = (*(u16*)(c + 0xc) == 7);
        if (b || *(s32*)(c + 8) != 0 || func_0203d9b4() != 0) {
            LoadTextNarcs();
            LoadArchive(0);
            LoadArchive(1);
            func_0200f220();
        }
    }

    {
        void* f = (void*)LoadFile(0x9807);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0, 0xe0);
        Deallocate(f);
    }
    {
        void* f = (void*)LoadFile(0x8406);
        _ZN3GXS10LoadBGPlttEPKvjj(f, 0xe0, 0x120);
        Deallocate(f);
    }

    *(u16*)0x4000008 = (*(u16*)0x4000008 & ~3) | 3;
    *(u16*)0x400000c = (*(u16*)0x400000c & ~3) | 2;
    *(u16*)0x400000e = (*(u16*)0x400000e & ~3) | 1;
    SetBg0Offset(0, 0);
    SetBg2Offset(0, 0);
    SetBg3Offset(0, 0);
    data_0209d45c = 0x11;

    *(u16*)0x4001008 = (*(u16*)0x4001008 & ~3) | 1;
    *(u16*)0x400100c = (*(u16*)0x400100c & ~3) | 3;
    *(u16*)0x400100e = (*(u16*)0x400100e & ~3) | 3;
    SetSubBg0Offset(0, 0);
    SetSubBg2Offset(0, 0);
    SetSubBg3Offset(0, 0);
    data_0209d454 = 0x18;

    *(u8*)(c + 0x283) = 0;
    *(u8*)(c + 0x284) = 0;
    *(u8*)(c + 0x285) = 0;

    if (data_0209b2e8 != 0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = 0;
    }
    *(u8*)(c + 0x287) = 0;
    LoadFont3D();

    {
        int b6 = (*(u16*)(c + 0xc) == 6);
        if (b6) {
            func_ov075_02116ad4(c);
            if (*(s32*)(c + 8) == 0) {
                if (func_0203da3c() == 0) {
                    data_0209b2e8 = (void*)LoadFile(0x8409);
                    UnloadArchive(0);
                    func_02030aa4(3);
                }
                {
                    int r = func_0203da9c();
                    func_02020028(r, data_0209f4ae[data_020a0e40 * 0x18]);
                }
            }
        } else {
            func_ov075_02116818(c);
        }
    }

    func_ov075_0211601c((char*)&data_ov075_0211d71c, (int)c, 1);
    data_0209d4a8 = &data_ov075_0211d71c;
    _ZN5Sound16LoadInitialGroupEi(0x2b);

    {
        int b6b = (*(u16*)(c + 0xc) == 6);
        if (b6b) {
            if (*(u32*)(c + 8) < 2) {
                _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x4c);
            }
        } else {
            if (data_0209b2f0[func_0203da9c()] != 0) {
                _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x51);
            } else {
                _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x52);
            }
        }
    }

    data_0208ee44 = 2;
    data_02092778 = 0;
    return 1;
}
