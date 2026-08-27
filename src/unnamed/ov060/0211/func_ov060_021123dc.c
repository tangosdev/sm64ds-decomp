extern unsigned char data_ov060_02119264[];
void func_ov060_021123dc(void* c){
  char* p = (char*)c;
  *(unsigned char*)(p+0x415)=1;
  *(unsigned char*)(p+0x41c)=0xff;
  *(unsigned char*)(p+0x41d)=0xff;
  if(*(unsigned char*)(p+0x414)==3) *(unsigned char*)(p+0x414)=0;
  *(unsigned char*)(p+0x41e)=data_ov060_02119264[*(unsigned char*)(p+0x414)];
  *(int*)(p+0x40c)=5;
  *(int*)(p+0x410)=0;
  *(unsigned short*)(p+0x420)=0;
  *(unsigned char*)(p+0x422)=0;
}
