//cpp
struct Vector3 { int x,y,z; };
struct Vector3_16 { short x,y,z; };
extern "C" {
extern void func_ov102_02149684(Vector3* out, void* self);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, Vector3*, Vector3_16*, int, int);
extern void func_ov102_0214ad14(void*);
extern void* func_ov102_0214b384(void*, int);
void* func_ov102_02149220(char* c){
  Vector3 v;
  func_ov102_02149684(&v, c);
  void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xce, 4, &v, 0, *(signed char*)(c+0xcc), -1);
  if(a == 0) return a;
  func_ov102_0214ad14(a);
  return func_ov102_0214b384(a, 0x3c);
}
}
