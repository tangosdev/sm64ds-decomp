extern int DecIfAbove0_Byte(char*);
void func_ov075_0211478c(char* c){
  int r=DecIfAbove0_Byte(c+0x156); if(r) return;
  *(short*)(c+0x150)=0; *(int*)(c+0x110)=3; *(int*)(c+0x114)=4;
}
