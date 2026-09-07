//cpp
extern "C" {
extern int func_ov007_020c3d1c(void *p);
int func_ov007_020c1180(char *c){
  if(*(unsigned char*)(c+0x1c)){
    func_ov007_020c3d1c(*(void**)(c+0x18));
  }
  return func_ov007_020c3d1c(c);
}
}
