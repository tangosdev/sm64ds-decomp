extern unsigned char data_ov066_0211abe0;
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae04;
extern int data_ov066_0211b0ac[];
extern int data_ov066_0211b0cc[];
extern int func_ov066_02119454(char* c, void* p);
int func_ov066_021188b0(char* c){
  if(data_ov066_0211abe0==0){
    *(unsigned short*)(c+0x400+0xd2)=0x64;
    func_ov066_02119454(c, data_ov066_0211b0ac);
    return 1;
  }
  if(data_ov066_0211ae08>=2){
    data_ov066_0211ae04=3;
    func_ov066_02119454(c, data_ov066_0211b0cc);
  }
  return 1;
}
