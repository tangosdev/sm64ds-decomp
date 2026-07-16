//cpp
extern "C" {
int _ZN5Model23AddToCommonModelDataArrER8BMD_File(void*);
unsigned int func_02046564(void*);
void* _ZN6Memory13operator_new2Ej(unsigned int);
int func_020462d0(void*, void*, void*);
int _ZN15ModelComponents21UpdateVertsUsingBonesEv(void*);
int func_02016b24(void*, int);
int _ZN5Model12SetPolygonIDEi(void*, int);

int _ZN5Model9DoSetFileEPcii(void* c, void* file, int a, int b){
  void* buffer;
  _ZN5Model23AddToCommonModelDataArrER8BMD_File(file);
  *(void**)((char*)c+0x4c) = _ZN6Memory13operator_new2Ej(func_02046564(file));
  buffer = *(void**)((char*)c+0x4c);
  if (buffer == 0) return 0;
  func_020462d0((char*)c+8, file, buffer);
  _ZN15ModelComponents21UpdateVertsUsingBonesEv((char*)c+8);
  if (a != 0) func_02016b24(c, 0x8000);
  if (b < 0) return 1;
  _ZN5Model12SetPolygonIDEi(c, b & 0xff);
  return 1;
}
}
