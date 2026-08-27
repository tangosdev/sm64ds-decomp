extern int func_ov002_020ae5c8(char* c, int x);
int func_ov002_020ae64c(char* c, int x){
  if(*(unsigned short*)(c+0x102) >= 5){
    int v=*(int*)(c+0x84) - 0x23d;
    if(v < 0x4cc) v=0x4cc;
    *(int*)(c+0x84)=v;
    *(int*)(c+0x88)=0x2000 - *(int*)(c+0x84);
    *(int*)(c+0x80)=*(int*)(c+0x88);
  }
  return func_ov002_020ae5c8(c, x);
}
