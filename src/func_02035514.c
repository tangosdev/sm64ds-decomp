extern int data_020991d8;
void func_02035514(char *c){
  *(int**)c = &data_020991d8;
  *(unsigned char*)(c+4) = 1;
  *(int*)(c+0xc) = 0;
  *(int*)(c+8) = -1;
}
