extern unsigned short data_ov006_0213bd64[];
extern int data_ov006_02141774[];
extern void Hud_RenderSprite(int,int,int,int,int);
void func_ov006_020d970c(unsigned char* c){
  int k;
  if(*(unsigned char*)(c+0x2b)==0) return;
  k=data_ov006_0213bd64[(*(unsigned char*)(c+0x2a)+1)*5+(*(int*)(c+0x24)>>12)];
  Hud_RenderSprite(
    data_ov006_02141774[k],
    (*(int*)(c+4)>>12)+0x18,
    (*(int*)(c+8)>>12)+0x18,
    -1,-1);
}
