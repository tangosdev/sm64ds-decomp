//cpp
extern "C" void func_ov007_020c33d0(void* a, int b, int c);
extern "C" void func_ov007_020c32e8(void* c, int sel, unsigned int n, int off);
extern "C" void func_ov007_020b3f20(void* p);
extern "C" void* func_ov007_020c99d8(void);
struct GX { static void LoadOBJ(void const*, unsigned int, unsigned int); };
extern "C" void func_ov007_020c3544(void* p);
extern "C" void func_ov007_020b3edc(int r0);
extern "C" char* data_ov007_0210342c;

extern "C" void func_ov007_020bcdb8(int idx)
{
    char* g = data_ov007_0210342c;
    char* s = *(char**)(g + 0x74);
    func_ov007_020c33d0((*(void***)(s + 0x24))[idx], 5, 0);
    func_ov007_020c32e8(*(void**)(*(char**)(s + 0x2c) + 4), 2, 0, 0);

    int k = *(unsigned char*)(*(char**)(data_ov007_0210342c + 0x28) + 0x3a) - 2;
    if (k < 0) return;

    char* obj = (char*)(idx + 0x17 + (k << 1));
    func_ov007_020b3f20(obj);
    void* m = func_ov007_020c99d8();
    func_ov007_020c33d0(m, 5, 0);
    GX::LoadOBJ(*(void**)m, 0, 0x2000);
    func_ov007_020c3544(m);
    func_ov007_020b3edc((int)obj);
}
