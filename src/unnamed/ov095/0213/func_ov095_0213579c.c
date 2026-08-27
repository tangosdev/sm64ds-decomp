extern int data_ov095_02136f58[];
int func_ov095_0213579c(void* self, char* p){
  int r0 = data_ov095_02136f58[*(int*)(p+8) & 3];
  unsigned eq = (unsigned)(*(unsigned short*)(p+0xc) == 0xbf);
  if(eq == 0) return r0;
  if(*(unsigned char*)(p+0x703) != 0) r0 = 0x2000;
  return r0;
}
