extern int func_0204f9c4();
extern unsigned char data_02075258[];
void func_02011c54(void){
  unsigned int i;
  unsigned char* p = data_02075258;
  for(i=0; i<11; i++){
    func_0204f9c4(*p, 1);
    p = p + 1;
  }
}
