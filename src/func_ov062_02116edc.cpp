//cpp
extern "C" {
int Math_Function_0203b14c(void*, int, int, int, int);
void* _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(void*, int, void*);
struct M48 { int w[12]; };
extern int data_ov062_0211df10[];
extern int data_ov062_0211df14[];
extern int data_ov062_0211df18[];
void func_ov062_02116edc(char* c){
  int idx = 0;
  if(*(int*)(*(int*)(c+0x3f8) + 8) == 2) idx = 1;
  int k = idx * 0xc;
  Math_Function_0203b14c(c+0x42c, *(int*)((char*)data_ov062_0211df10 + k), 0x800, 0x3e8000, 4);
  Math_Function_0203b14c(c+0x430, *(int*)((char*)data_ov062_0211df14 + k), 0x800, 0x3e8000, 4);
  Math_Function_0203b14c(c+0x434, *(int*)((char*)data_ov062_0211df18 + k), 0x800, 0x3e8000, 4);
  void* r = _ZN5Actor11UpdateCarryER6PlayerRK7Vector3(c, *(int*)(c+0x3f8), c+0x42c);
  *(struct M48*)(c+0x31c) = *(struct M48*)r;
}
}
