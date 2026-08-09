//cpp
extern "C" {
extern void func_ov007_020c8440(void *p);
extern void func_ov007_020c8098(void *p);
extern void func_ov007_020c78dc(void *p);
extern int func_ov007_020c3d1c(void *p);

void func_ov007_020c9fa0(char *c){
  int i;
  for(i = 0; i < 2; i++){
    func_ov007_020c8440(((void**)(c + 0x50))[i]);
    func_ov007_020c8440(((void**)(c + 0x58))[i]);
    func_ov007_020c8098(((void**)(c + 0x60))[i]);
    func_ov007_020c8098(((void**)(c + 0x68))[i]);
  }
  func_ov007_020c78dc(*(void**)(c+0x1c));
  func_ov007_020c3d1c(*(void**)(c+0x18));
  func_ov007_020c3d1c(c);
}
}
