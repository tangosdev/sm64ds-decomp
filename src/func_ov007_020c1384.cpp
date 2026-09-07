//cpp
extern "C" {
extern int func_ov007_020c3d1c(void *p);
int func_ov007_020c1384(char *c){
  func_ov007_020c3d1c(*(void**)(c));
  return func_ov007_020c3d1c(*(void**)(c+4));
}
}
