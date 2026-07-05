//cpp
extern "C" {
typedef int Fix12i;
struct Camera;
extern int* data_0209f318;
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, Fix12i d, unsigned int e);
extern void func_ov002_020e25f0(void* c, int i);
extern void func_0200d678(Camera* thiz, unsigned char pid);
int _ZN6Player12St_Spin_InitEv(char* c){
  *(unsigned char*)(c+0x71b) = 0;
  *(unsigned char*)(c+0x712) = 1;
  *(unsigned char*)(c+0x70d) = 0;
  *(unsigned char*)(c+0x6e1) = 0;
  *(unsigned char*)(c+0x6de) = 1;
  *(unsigned char*)(c+0x6df) = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x5f, 0, 0x1000, 0);
  *(int*)(c+0xa0) = -0x10000;
  if (*(unsigned char*)(c+0x6e6)) {
    *(int*)(c+0xa8) = 0x50000;
    func_ov002_020e25f0(c, 2);
  }
  int* p = (int*)(((int)c + 0x2ec) & 0xFFFFFFFFFFFFFFFFull);
  int old = *p;
  int** cam_ptr_ptr = &data_0209f318;
  *p = old | 0x20;
  unsigned char pid = *(unsigned char*)(c+0x6d8);
  func_0200d678((Camera*)*cam_ptr_ptr, pid);
  return 1;
}
}
