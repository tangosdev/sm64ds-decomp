extern int _ZN5Model8LoadFileER13SharedFilePtr();
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii();
extern int func_ov044_02111214();
extern int data_ov059_02111680[];
int _ZN19OrangeBallBillboard13InitResourcesEv(char *c){
  int f = _ZN5Model8LoadFileER13SharedFilePtr((int)data_ov059_02111680);
  _ZN9ModelBase7SetFileEP8BMD_Fileii((char*)c+0xd4, f, 1, -1);
  func_ov044_02111214(c);
  return 1;
}
