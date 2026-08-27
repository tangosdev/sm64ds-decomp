extern void func_020178cc();
extern void func_020731dc();
extern void func_020178b4(void);
extern int data_ov075_0211d980[];
extern int data_ov075_0211d988[];
struct P2 { int a, b; };
extern struct P2 data_ov075_0211d354;
extern struct P2 data_ov075_0211d34c;
extern struct P2 data_ov075_0211d35c;
extern struct P2 data_ov075_0211d994[3];
void __sinit_ov075_0211c51c(void){
  func_020178cc(data_ov075_0211d980, 0x8418);
  func_020731dc(data_ov075_0211d980, &func_020178b4, data_ov075_0211d988);
  data_ov075_0211d994[0] = data_ov075_0211d354;
  data_ov075_0211d994[1] = data_ov075_0211d34c;
  data_ov075_0211d994[2] = data_ov075_0211d35c;
}
