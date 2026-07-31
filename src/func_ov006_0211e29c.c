extern void func_ov004_020b2220(int,int,int,int,int,int,int);
extern int func_ov004_020ad674(void);
extern void DrawOamSprite(int,int,int,int);
extern int* data_ov006_0213f9d0[];
void func_ov006_0211e29c(unsigned char* c){
  if(*(unsigned char*)(c+0x4c1b)==0) return;
  func_ov004_020b2220(0x80,0x60,*(unsigned short*)(c+0x4c14),1,0,0x800,0);
  DrawOamSprite(data_ov006_0213f9d0[func_ov004_020ad674()][3],0x80,0x48,0);
}
