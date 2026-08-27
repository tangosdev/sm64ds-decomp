extern int RandomIntInternal(void*);
extern int data_0209e650[];
int func_ov032_02111f9c(char* c){
  unsigned int r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x430) = ((r>>8)&0xf)<<0xc;
  r = RandomIntInternal(data_0209e650);
  *(short*)(c+0x100) = ((r>>8)&0x3f)+0x32;
  *(int*)(c+0x3a8) = 0x1000;
  return 1;
}
