extern void func_ov002_020e9464(char *p);
extern void func_ov002_020e7e24(char *thiz);
extern void func_ov002_020e81e0(char *p);
extern void func_ov002_020e7d08(char *p);
void func_ov002_020ea7ac(char* c){
  if(*(int*)(c+0xa8) <= -0x18000){
    func_ov002_020e9464(c);
    *(int*)(((int)c + 0x128)) &= ~1;
  }else{
    if(*(int*)(c+0x9c) == 0){
      *(int*)(c+0x440) = 4;
      *(unsigned short*)(c+0x400+0x96) = 0x1d6;
    }else{
      func_ov002_020e7e24(c);
    }
  }
  func_ov002_020e81e0(c);
  func_ov002_020e7d08(c);
}
