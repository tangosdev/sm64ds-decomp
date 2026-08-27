extern int func_ov002_020c04ac(char*c);
int func_ov002_020c0434(char* c){
  unsigned short f = *(unsigned short*)(c+0x600+0xce) & 1;
  if(f){
    if(*(int*)(c+0x658)==5) return 1;
    if(*(int*)(c+0x98)<0) return 1;
    if(func_ov002_020c04ac(c)) return 1;
  }
  return 0;
}
