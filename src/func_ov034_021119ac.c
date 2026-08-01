extern int _Z14ApproachLinearRiii(int*,int,int);
extern int _ZN6Player12GetTalkStateEv(int);
extern int data_ov052_021125b8();
void func_ov034_021119ac(int c){
  _Z14ApproachLinearRiii((int*)((char*)c+0x98),0,0x1000);
  if(_ZN6Player12GetTalkStateEv(*(int*)((char*)c+0x8c8))!=2) return;
  if(*(unsigned char*)((char*)c+0x8db)>1){
    data_ov052_021125b8(c,3);
    return;
  }
  data_ov052_021125b8(c,7);
}
