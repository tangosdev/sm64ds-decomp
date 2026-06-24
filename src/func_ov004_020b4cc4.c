typedef struct { int x, y; } Vec2_Fix12;
extern void func_0203d6d0(int* o, int* a, int* b);
extern void func_0203d680(Vec2_Fix12* out, const Vec2_Fix12* in, int scale);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc818;

void func_ov004_020b4cc4(char* r4){
  Vec2_Fix12 v1;
  Vec2_Fix12 v2;
  int a, b;
  *(int*)(r4 + 0x10) = 0xc000;
  *(int*)(r4 + 0x14) = 0xc000;
  func_0203d6d0((int*)&v1, (int*)(r4 + 0x10), (int*)(r4 + 8));
  func_0203d680(&v2, &v1, 0xc0);
  *(int*)(r4 + 0x18) = v2.x;
  *(int*)(r4 + 0x1c) = v2.y;
  a = *(int*)(r4 + 0x18);
  if (a < 0) a = -a;
  *(int*)(r4 + 0x18) = a;
  b = *(int*)(r4 + 0x1c);
  if (b < 0) b = -b;
  *(int*)(r4 + 0x1c) = b;
  *(struct P2*)r4 = data_ov004_020bc818;
}
