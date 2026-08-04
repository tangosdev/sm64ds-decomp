extern int data_ov006_02141f8c[];

typedef void (*Fn)(void*, int);

#pragma opt_common_subs off
void func_ov006_020e8830(char* base, int idx){
  unsigned char b = *(unsigned char*)(base + idx*0x20 + 0x52da);
  int* ent = &data_ov006_02141f8c[b*2];
  int adj = ent[1];
  void* obj = base + (adj >> 1);
  Fn fn;
  if(adj & 1){
    void* vt = *(void**)obj;
    fn = *(Fn*)((char*)vt + ent[0]);
  }else{
    fn = (Fn)ent[0];
  }
  fn(obj, idx);

  unsigned char c = *(unsigned char*)(base + idx*0x20 + 0x52d6);
  *(int*)(base + 0x52c8 + idx*0x20) -= (c<<3) + 0x10;
  int sh = *(unsigned short*)(base + idx*0x20 + 0x52cc) >> 3;
  if(sh >= 7) sh = 7;
  *(unsigned char*)(base + idx*0x20 + 0x52d7) = 7 - sh;
  if(*(unsigned short*)(base + idx*0x20 + 0x52cc) != 0){
    *(unsigned short*)(base + 0x52cc + idx*0x20) -= 1;
    if(*(short*)(base + idx*0x20 + 0x52cc) < 0) *(short*)(base + idx*0x20 + 0x52cc) = 0;
  }else{
    *(unsigned char*)(base + idx*0x20 + 0x52d9) = 0;
  }
}
