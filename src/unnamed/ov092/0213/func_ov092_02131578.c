extern int func_ov092_021314d0(char*);
extern int func_ov092_021319b0(char*);
void func_ov092_02131578(char* c){
  *(unsigned char*)(c+0x576) = 0;
  if(*(unsigned short*)(c+0x564) == 0x14){
    func_ov092_021314d0(c);
  }
  func_ov092_021319b0(c);
}
