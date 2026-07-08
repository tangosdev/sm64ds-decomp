//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern "C" {
int LoadFile(int handle);
void DecompressLZ16(int src, void* dst);
void func_ov004_020adc5c(int handle);
void Deallocate(void* ptr);
void func_020562b4(const void* src, u32 offset, u32 count);
extern u8 BOTTOM_SCREEN_RELATED;
}
namespace G2S { void* GetBG3CharPtr(); }
namespace GXS { void LoadBGPltt(const void*, u32, u32); }

extern "C" void func_ov006_0210a534(void){
  int h = LoadFile(0x26);
  BOTTOM_SCREEN_RELATED |= 8;
  *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & 0x43) | 0x214;
  DecompressLZ16(h, G2S::GetBG3CharPtr());
  func_ov004_020adc5c(h);
  h = LoadFile(0x27);
  GXS::LoadBGPltt((void*)h, 0xa0, 0x160);
  Deallocate((void*)h);
  h = LoadFile(0x28);
  func_020562b4((void*)h, 0, 0x800);
  Deallocate((void*)h);
  *(volatile u16*)0x400100e &= ~0x40;
  *(volatile u32*)0x400101c = 0;
  *(volatile u16*)0x400100e = (*(volatile u16*)0x400100e & ~3) | 1;
}
