//cpp
extern "C" {
extern void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern void func_ov044_02111214(char*);
extern int* data_ov031_02111424[];
int _ZN25SlideDecorationSilverStar13InitResourcesEv(char *c){
  unsigned short type = *(unsigned short*)(c+0xc);
  switch(type){
    case 0x12e: *(char*)(c+0x124)=0; break;
    case 0x12f: *(char*)(c+0x124)=1; break;
    case 0x130: *(char*)(c+0x124)=2; break;
    case 0x131: *(char*)(c+0x124)=3; break;
  }
  unsigned char idx = *(unsigned char*)(c+0x124);
  void *f = _ZN5Model8LoadFileER13SharedFilePtr((void*)data_ov031_02111424[idx]);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, -1);
  func_ov044_02111214(c);
  return 1;
}
}
