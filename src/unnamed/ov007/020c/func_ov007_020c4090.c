void func_ov007_020c4090(char* c){
  *(short*)(c+8) = -1;
  *(short*)(c+6) = 0;
  *(short*)(c+4) = *(short*)(c+6);
  char* p = *(char**)(c);
  *(int*)(p+8) = 0;
}
