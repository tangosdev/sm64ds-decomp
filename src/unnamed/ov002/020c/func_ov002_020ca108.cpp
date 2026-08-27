//cpp
extern "C" {
extern int data_ov002_0211013c[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
void func_ov002_020ca108(char* c){
  if(*(unsigned char*)(c+0x70b)){
    *(unsigned char*)(c+0x6e3)=0x13;
    return;
  }
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_0211013c);
}
}
