extern int func_ov007_020c937c();
extern int func_ov007_020c3d1c();
int func_ov007_020ca26c(char* c){
  int i;
  for(i=0;i<4;i++){
    func_ov007_020c937c(((int*)(c+8))[i]);
  }
  return func_ov007_020c3d1c(c);
}
