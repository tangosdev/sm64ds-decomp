extern int RenderOamMainScreen(int,int,int,int,int);
extern int data_ov090_021343b0[];
void func_ov006_020d5e5c(int c){
  c += 0x6000;
  if(*(unsigned char*)(c+0x2af)==0) return;
  RenderOamMainScreen(data_ov090_021343b0[*(unsigned char*)(c+0x2ae)],
    *(int*)(c+0x2a0)>>0xc, *(int*)(c+0x2a4)>>0xc, -1, 2);
}
