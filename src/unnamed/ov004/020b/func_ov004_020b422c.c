extern int data_ov004_020bc150;
extern void func_02012790(int x);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc224;
extern struct P2 data_ov004_020bc274;

void func_ov004_020b422c(char* r4){
  data_ov004_020bc150 = 0;
  switch (*(int*)(r4 + 0x20)) {
  case 0:
    func_02012790(0x62);
    break;
  case 1:
  case 2:
    func_02012790(0x63);
    break;
  }
  *(int*)(r4 + 0x24) = 0;
  *(int*)(r4 + 0x28) = 0x10000;
  *(struct P2*)r4 = data_ov004_020bc224;
  *(struct P2*)(r4 + 8) = data_ov004_020bc274;
}
