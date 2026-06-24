typedef struct { int x, y; } V2;
extern void func_0203d6d0(V2* o, int* a, int* b);
extern void func_0203d680(V2* out, V2* in, int scale);
extern int data_ov004_020bc808[];

void func_ov004_020b4b84(char* c, int* in){
  V2 a;
  int pad[2];
  V2 b;
  *(int*)(c+8) = in[0];
  *(int*)(c+0xc) = in[1];
  *(short*)(c+0x20) = 0;
  *(unsigned char*)(c+0x22) = 1;
  *(int*)(c+0x10) = 0xc000;
  *(int*)(c+0x14) = 0xc000;
  func_0203d6d0(&a, (int*)(c+0x10), (int*)(c+8));
  func_0203d680(&b, &a, 0x100);
  *(int*)(c+0x18) = b.x;
  *(int*)(c+0x1c) = b.y;
  {
    int x = *(int*)(c+0x18);
    if (x < 0) x = -x;
    *(int*)(c+0x18) = x;
  }
  {
    int y = *(int*)(c+0x1c);
    if (y < 0) y = -y;
    *(int*)(c+0x1c) = y;
  }
  *(V2*)c = *(V2*)data_ov004_020bc808;
}
