extern int DecIfAbove0_Byte(char*);
void func_ov091_02132e64(char* c){
  int r = DecIfAbove0_Byte(c+0x39e);
  if(r==0){
    *(int*)(c+0x398) = 0;
    *(unsigned char*)(c+0x39e) = 0x28;
  }
}
