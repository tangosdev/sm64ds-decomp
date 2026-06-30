typedef unsigned char u8;
typedef unsigned int u32;

extern int LoadFile(int handle);
extern int func_02054d88(void);
extern void DecompressLZ16(int dst, int src);
extern void func_02056314(int src, u32 offset, u32 count);
extern void GX_LoadBGPltt(const void *p, u32 a, u32 b);
extern void Deallocate(void *ptr);
extern int GetBG3CharPtr(void);
extern void func_020562b4(int src, u32 offset, u32 count);
extern void GXS_LoadBGPltt(const void *p, u32 a, u32 b);

extern u8 data_0209d45c;
extern u8 data_0209d454;

void func_ov006_020edcb0(void){
  int r6 = LoadFile(0x3b);
  int r5 = LoadFile(0x3d);
  int r4 = LoadFile(0x3c);
  {
    volatile unsigned short *m = (volatile unsigned short *)0x400000e;
    *m = (*m & ~3) | 1;
    *m = (*m & 0x43) | 0x614;
  }
  DecompressLZ16(r6, func_02054d88());
  func_02056314(r5, 0, 0x800);
  GX_LoadBGPltt((const void *)r4, 0x60, 0x1a0);
  data_0209d45c |= 8;
  Deallocate((void *)r6);
  Deallocate((void *)r5);
  Deallocate((void *)r4);

  r6 = LoadFile(0x3e);
  r5 = LoadFile(0x40);
  r4 = LoadFile(0x3f);
  {
    volatile unsigned short *m = (volatile unsigned short *)0x400100e;
    *m = (*m & ~3) | 2;
    *m = (*m & 0x43) | 0x614;
  }
  DecompressLZ16(r6, GetBG3CharPtr());
  func_020562b4(r5, 0, 0x800);
  GXS_LoadBGPltt((const void *)r4, 0x60, 0x1a0);
  data_0209d454 |= 8;
  Deallocate((void *)r6);
  Deallocate((void *)r5);
  Deallocate((void *)r4);
}
