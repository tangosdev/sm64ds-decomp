extern void func_0204f94c(void*);
void func_02011a28(char* c){
  char* p = c + 8;
  int i;
  for(i=0;i<0x40;i++){
    func_0204f94c(p+8);
    p += 0x14;
  }
}
