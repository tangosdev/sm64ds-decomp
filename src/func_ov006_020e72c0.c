typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

int func_02053ea0(void);
s32 func_ov004_020ad674(void);
u32 LoadCompressedFileAt(unsigned int fileID, void *target);
void DecompressLZ16(const void *src, void *dst);
int func_02053eb0(void);
void SetSubBg1Offset(int a, int b);
unsigned int func_02054e88(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned int);
void _ZN2GX15SetBankForSubBGEt(unsigned int);
void *_ZN3G2S12GetBG1ScrPtrEv(void);

extern unsigned int data_ov006_0213c5fc[];
extern unsigned int data_ov006_0213c5e8[];
extern unsigned int data_ov006_0213c610[];
extern unsigned char BOTTOM_SCREEN_RELATED;

void func_ov006_020e72c0(char *c) {
  *(int*)(c + 0x4660) = func_02053ea0();
  _ZN2GX16SetBankForSubOBJEt(0x100);
  LoadCompressedFileAt(data_ov006_0213c5fc[func_ov004_020ad674()], (void*)0x6600000);
  {
    char *dst = (char*)0x6600000; dst += 0x2000;
    DecompressLZ16((void*)data_ov006_0213c5e8[func_ov004_020ad674()], dst);
  }
  _ZN2GX16SetBankForSubOBJEt(*(int*)(c + 0x4660));
  *(int*)(c + 0xa0) = func_02053eb0();
  _ZN2GX15SetBankForSubBGEt(0x80);
  {
    volatile u16 *p = (volatile u16*)0x400100a;
    *p = (*p & 0x43) | 0x800;
    *p = *p & ~3;
    *p = *p & ~0x40;
  }
  SetSubBg1Offset(0, 0);
  BOTTOM_SCREEN_RELATED &= ~2;
  {
    s32 i = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov006_0213c610[i], (void*)func_02054e88());
  }
  LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
  _ZN2GX15SetBankForSubBGEt(*(int*)(c + 0xa0));
}
