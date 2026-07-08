extern int func_02011ee4(int x);
extern unsigned char CURRENT_GAMEMODE;
extern unsigned char data_ov002_020ff0f4[];
void func_ov002_020e6350(char* c){
  int b = (int)(CURRENT_GAMEMODE == 1);
  if(b != 0) return;
  func_02011ee4(data_ov002_020ff0f4[*(int*)(c+8) & 3]);
}
