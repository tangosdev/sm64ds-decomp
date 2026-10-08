extern void func_ov075_0211a194(void* c, void* arg);
extern int data_ov075_0211d840[];
struct P2 { int a, b; };
void func_ov075_0211a148(char* c, int a, int b){
  volatile struct P2 s;
  *(int*)(c+0x6c) = a;
  *(int*)(c+0x264) = b;
  s = *(volatile struct P2*)(c+0x64);
  func_ov075_0211a194(c, data_ov075_0211d840);
  *(struct P2*)(c+0x64) = *(struct P2*)&s;
}
