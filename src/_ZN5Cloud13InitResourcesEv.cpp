//cpp
extern int data_ov039_021118e4[];
extern int data_ov041_021118e0;
extern "C" void* _ZN5Model8LoadFileER13SharedFilePtr(void*);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void*, void*, int, int);
extern "C" void func_ov044_02111214(char *t);
extern "C" int _ZN5Cloud13InitResourcesEv(char *c){
  void *f = _ZN5Model8LoadFileER13SharedFilePtr(data_ov039_021118e4);
  _ZN9ModelBase7SetFileEP8BMD_Fileii(c+0xd4, f, 1, 2);
  func_ov044_02111214(c);
  data_ov041_021118e0++;
  return 1;
}
