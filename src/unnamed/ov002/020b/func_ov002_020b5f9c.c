enum Bool { FALSE, TRUE };
void func_ov002_020b5f9c(char *a, char *b){
  unsigned short t = *(unsigned short*)(b+0xc);
  enum Bool eq = (enum Bool)(t==0xbf);
  if(eq){
    *(int*)(a+0x33c)=(int)b;
    *(short*)(a+0x342)=5;
  }
}
