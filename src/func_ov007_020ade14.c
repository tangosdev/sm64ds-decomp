extern int func_ov007_020c1180();
extern int func_ov007_020aea4c();
extern int func_ov007_020c3d1c();
int func_ov007_020ade14(char* c){
  if(*(unsigned short*)(*(int*)c)==0)
    func_ov007_020c1180(*(int*)(*(int*)(c+4)+0x38));
  func_ov007_020aea4c(*(int*)c);
  func_ov007_020c1180(*(int*)(c+4));
  return func_ov007_020c3d1c(c);
}
