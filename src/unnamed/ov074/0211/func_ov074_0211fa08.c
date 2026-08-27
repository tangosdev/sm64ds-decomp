extern int func_01ffa344(int);
extern void func_ov074_02121a4c(char*, int, int);
extern int data_ov074_02122e4c[];
extern unsigned short data_ov074_02122e04[];
void func_ov074_0211fa08(char* c){
  *(char*)(c+0x604)=0;
  int v=func_01ffa344(data_ov074_02122e4c[1]);
  *(int*)(c+0x80)=v;
  *(int*)(c+0x84)=v;
  *(int*)(c+0x88)=v;
  *(int*)(c+0x3ec)=0x2000;
  *(int*)(c+0x3d8)=((int)data_ov074_02122e04[1])<<0xc;
  *(char*)(c+0x603)=0;
  func_ov074_02121a4c(c, 3, 0);
  *(char*)(c+0x607)=3;
}
