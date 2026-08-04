int func_ov006_02111dcc(char *p, int val){
  if(*(unsigned char*)(p+0x30) == 0) return 0;
  if(*(int*)(p+0x40) > 0) return 0;
  *(int*)(p+0x40) = val;
  return 1;
}
