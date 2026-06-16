extern int ApproachLinear(int*,int,int);
extern int Player_GetTalkState(int);
extern int data_ov052_021125b8();
void func_ov034_021119ac(int c){
  ApproachLinear((int*)((char*)c+0x98),0,0x1000);
  if(Player_GetTalkState(*(int*)((char*)c+0x8c8))!=2) return;
  if(*(unsigned char*)((char*)c+0x8db)>1){
    data_ov052_021125b8(c,3);
    return;
  }
  data_ov052_021125b8(c,7);
}
