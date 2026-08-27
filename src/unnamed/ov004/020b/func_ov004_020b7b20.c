void FreeGfxSlotsById(int x);
struct P2 { int a, b; };
extern struct P2 data_ov004_020bc93c;
extern int data_ov004_020bc7d4;
extern int data_ov004_020bfa24;
void func_ov004_020b7b20(char* c){
  int v = *(int*)(c+0x1c);
  if(v > 0x64) return;
  if(v != 0) return;
  FreeGfxSlotsById(0x1d);
  *(struct P2*)(c+8) = data_ov004_020bc93c;
  *(int*)(c+0x24) = 1;
  data_ov004_020bc7d4 = 0;
  data_ov004_020bfa24 = 0;
}
