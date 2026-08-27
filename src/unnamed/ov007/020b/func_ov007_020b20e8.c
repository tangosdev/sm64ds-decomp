extern char* data_ov007_0210342c;
extern void func_ov007_020c0128(char* c);
extern void func_ov007_020bffb8(char* c);
extern void func_ov007_020c0078(char* c);
extern void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(void* a, void* b, void* c, int d, void* e);
void func_ov007_020b20e8(void){
  char* g = data_ov007_0210342c;
  char* t = *(char**)(g + 0x34);
  func_ov007_020c0128(t);
  *(int*)(t+0x2c) = 0x8000;
  *(short*)(t+0xd4) = 0x4da;
  *(int*)(t+0xb4) = 0x12bb;
  func_ov007_020bffb8(t);
  _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(t+8, t+0x20, t+0x14, 1, t+0x44);
  func_ov007_020c0078(t);
}
