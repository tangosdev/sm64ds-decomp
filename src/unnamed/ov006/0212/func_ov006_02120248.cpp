//cpp
// @symbol func_ov006_02120248
// recovered name: dScMgTeresa_c_OnYoshiTryEat_02120248
/* recovered: renamed to Class_Method */
extern "C" {
void func_ov006_0211fbf8(char* p);
void func_ov006_0211dd6c(char* p);
void func_ov006_0211d7b4(char* p);
void func_ov006_0211f77c(char* p);
void MultiStore16(unsigned short val, char* dst, int nbytes);
void FreeGfxSlotsById(int arg);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
int LoadFile(int handle);
int DecompressLZ16(int handle, int dst);
void Deallocate(void* ptr);
}
struct G2S { static char* GetBG0CharPtr(); };

extern "C" void func_ov006_02120248(char* self, int reset)
{
    volatile unsigned short val;
    if (reset == 0) {
        unsigned int v = *(unsigned int*)(self + 0xbc) + 1;
        *(unsigned int*)(self + 0xbc) = v;
        if (*(unsigned int*)(self + 0xbc) > 0x270e)
            *(unsigned int*)(self + 0xbc) = 0x270e;
    } else {
        *(int*)(self + 0xb4) = 0;
        *(unsigned int*)(self + 0xbc) = 0;
        if (*(unsigned int*)(self + 0xbc) > 0x270e)
            *(unsigned int*)(self + 0xbc) = 0x270e;
    }
    func_ov006_0211fbf8(self);
    func_ov006_0211dd6c(self);
    func_ov006_0211d7b4(self);
    func_ov006_0211f77c(self);
    char* dst = G2S::GetBG0CharPtr();
    val = 0x1111;
    MultiStore16(val, dst, 0x6000);
    *(int*)(self + 0x4be8) = 0;
    FreeGfxSlotsById(0x1d);
    *(short*)(self + 0x4c16) = 0x20;
    func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    int h = LoadFile(0x101);
    DecompressLZ16(h, 0x6400000);
    Deallocate((void*)h);
}
