extern int func_ov002_020c031c(void* c);
int func_ov002_020c0cbc(char* c){
  int t;
  int d;
  if((*(unsigned char*)(c+0x6e9) & 1) == 0) return 0;
  d = *(int*)(c+0x644) - *(int*)(c+0x60);
  if(d < 0) d = -d;
  if(d >= 0x50000) return 0;
  switch(func_ov002_020c031c(c)){
    case 4: t = 0xfc1; break;
    case 5: t = 0xfff; break;
    case 3: t = 0xf04; break;
    default:
    case 0: t = 0xc9b; break;
    case 1:
    case 2: t = 0; break;
  }
  if(*(int*)(c+0x558) <= t){
    (*(unsigned short *)(((int)c + 0x6ce) & 0xFFFFFFFFFFFFFFFF)) |= 1;
    return 1;
  }
  return 0;
}
