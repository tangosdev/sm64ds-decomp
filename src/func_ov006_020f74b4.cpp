//cpp
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern "C" {
void func_ov006_0210a534(void);
s32 func_ov004_020ad674(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
void func_ov006_020f7064(void *c);
void func_ov006_020c0aa8(void *c);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b66d4(void);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
}
namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int func_020bc7d4;
extern int data_ov006_0213d370[];

extern "C" int func_ov006_020f74b4(char *self)
{
    void *a;
    void *b;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    a = LoadFile(data_ov006_0213d370[func_ov004_020ad674()]);
    b = LoadFile(0xbb);
    DecompressLZ16(a, (void *)0x6400000);
    GX::LoadOBJPltt(b, 0, 0x100);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0x100);
    Deallocate(a);
    Deallocate(b);
    data_0209d454 = 0x18;
    func_ov006_020f7064(self);
    func_ov006_020c0aa8(self + 0x4660);
    if (func_ov006_020c1a88(self + 0x4f38) == 0) return 0;
    *(int *)(self + 0xa8) = func_ov004_020ad8b8();
    *(int *)(self + 0xac) = *(int *)(self + 0xa8);
    *(int *)(self + 0xb4) = func_ov004_020ad878();
    func_ov004_020b66d4();
    func_020bc7d4 = 1;
    *(u8 *)(self + 0x5409) = 5;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}
