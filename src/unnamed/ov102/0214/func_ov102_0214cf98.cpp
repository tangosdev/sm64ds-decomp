//cpp
extern "C" {
extern int data_ov102_0214ea48[];
extern int func_ov102_0214d1f8(void *c, void *p);
int func_ov102_0214cf98(char *c, int p){
  if(*(int*)(c+0x3c0)!=0) return 0;
  *(int*)(c+0x3c0)=p;
  if(*(int*)(c+0x3ac)!=(int)data_ov102_0214ea48)
    func_ov102_0214d1f8(c, data_ov102_0214ea48);
  return 1;
}
}
