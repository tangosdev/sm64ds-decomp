extern unsigned char* func_ov010_0211139c(int c);
void func_ov010_02111284(int c){
  unsigned char* r=func_ov010_0211139c(c);
  if(r==0) return;
  if(r[0x3aa]==0) *(int*)((char*)c+0x3a0)=3;
}
