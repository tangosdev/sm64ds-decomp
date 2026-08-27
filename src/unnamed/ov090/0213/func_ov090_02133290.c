extern int RandomIntInternal(void*);
extern int data_0209e650[];
int func_ov090_02133290(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x384) = ((r>>8)&0xf)<<0xc;
  r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x3f)+0x32;
  *(int*)(c+0x98) = 0x5000;
  return 1;
}
