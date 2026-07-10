typedef unsigned char u8;
typedef unsigned int u32;
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
extern int _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void*,int,int,int);
extern int _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void*,int,int,int,unsigned int);
extern int data_ov100_021489cc[];
extern int* data_ov100_021473a4[];
extern int* data_ov100_021473b0[];
int _ZN4Fish13InitResourcesEv(char* c){
  u8 v;
  _ZN9Animation8LoadFileER13SharedFilePtr(data_ov100_021489cc);
  *(u8*)(c+0x15b) = (*(u32*)(c+8) >> 4) & 7;
  v = *(u8*)(c+0x15b);
  if (v > 2) {
    if (v < 6) *(u8*)(c+0x15c) = v - 2;
    *(u8*)(c+0x15b) = 0;
  }
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, _ZN5Model8LoadFileER13SharedFilePtr(data_ov100_021473a4[*(u8*)(c+0x15b)]), 1, -1);
  _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(c+0xd4, _ZN9Animation8LoadFileER13SharedFilePtr(data_ov100_021473b0[*(u8*)(c+0x15b)]), 0, 0x1000, 0);
  *(u8*)(c+0x159) = 1;
  *(int*)(c+0x13c) = *(int*)(c+4);
  *(int*)(c+0x14c) = 0;
  *(int*)(c+0x140) = *(int*)(c+0x60) + 0xc8000;
  *(int*)(c+0x150) = 0;
  return 1;
}
