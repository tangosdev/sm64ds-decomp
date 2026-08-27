extern void func_ov075_0211a194(void* self, void* g);
extern int data_ov075_0211d858[];

struct P2 { int a, b; };

void func_ov075_0211a100(char* c, int r1){
  struct P2 t;
  *(int*)(c+0x6c) = r1;
  t = *(struct P2*)(c+0x64);
  func_ov075_0211a194(c, data_ov075_0211d858);
  *(struct P2*)(c+0x64) = t;
}
