
extern void func_02017acc();
extern void func_020731dc();
extern void func_02017934();
extern int data_ov072_02122c40[];
extern int data_ov072_02122c48[];
extern int data_ov072_02122c50[];
extern int func_02017ab4[];
extern int func_0201791c[];
extern int func_020072c0[];
extern int data_ov072_02122c88[];
extern int data_ov072_02122c7c[];
extern int data_ov072_02122c58[];
extern int data_ov072_02122c64[];
extern int data_ov072_02122c70[];
void __sinit_ov072_02122350(void)
{
  int new_var;
  func_02017acc(data_ov072_02122c40, 0x2ad);
  func_020731dc(data_ov072_02122c40, func_02017ab4, data_ov072_02122c88);
  func_02017acc(data_ov072_02122c48, 0x2af);
  func_020731dc(data_ov072_02122c48, func_02017ab4, data_ov072_02122c7c);
  func_02017934(data_ov072_02122c50, 0x2b0);
  func_020731dc(data_ov072_02122c50, func_0201791c, data_ov072_02122c58);
  data_ov072_02122c70[new_var] = (new_var = 0);
  data_ov072_02122c70[1] = -0x50000;
  data_ov072_02122c70[2] = new_var;
  func_020731dc(data_ov072_02122c70, func_020072c0, data_ov072_02122c64);
}
