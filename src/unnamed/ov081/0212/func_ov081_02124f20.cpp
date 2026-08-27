//cpp
extern "C" {
extern void func_ov081_02125488(void*, void*);
extern void func_ov081_0212423c(void*, int);
extern void* data_ov081_02128e14[];
int func_ov081_02124f20(char *c){
  if(*(int*)(c+0xa8) < 0){
    if(*(int*)(c+0x450) > *(int*)(c+0x60)){
      *(int*)(c+0xa8)=0;
      *(int*)(c+0x9c)=0;
      *(int*)(c+0x60)=*(int*)(c+0x450);
      func_ov081_02125488(c, data_ov081_02128e14);
    }
  }
  func_ov081_0212423c(c, 0);
  return 1;
}
}
