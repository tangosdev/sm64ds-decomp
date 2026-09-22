extern int func_ov007_020c3d1c();
extern int func_ov007_020c937c();
int func_ov007_020adbc8(char* c){
  unsigned char x = *(unsigned char*)(c+8);
  if(x==1 || x==5)
    func_ov007_020c3d1c(*(int*)(*(int*)(c+4)+0x10));
  func_ov007_020c937c(*(int*)c);
  return func_ov007_020c3d1c(c);
}
