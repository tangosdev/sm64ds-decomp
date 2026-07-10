//cpp
extern "C" {
extern int data_ov075_0211d304[];
extern int func_ov075_02115bc8[];
extern int func_ov075_02115bac[];
extern int _ZTV5Stage[];
extern int data_0208e4b8[];
int func_0207328c(void* a, int b, int c, void* d);
int _ZN9ActorBaseD1Ev(void* c);
int func_ov075_02115ab8(void* c){
  char* p=(char*)c;
  *(int*)p=(int)data_ov075_0211d304;
  func_0207328c(p+0x1b4, 4, 0x2c, func_ov075_02115bc8);
  func_0207328c(p+0x70, 9, 0x24, func_ov075_02115bac);
  *(int*)p=(int)_ZTV5Stage;
  *(int*)p=(int)data_0208e4b8;
  _ZN9ActorBaseD1Ev(c);
  return (int)c;
}
}
