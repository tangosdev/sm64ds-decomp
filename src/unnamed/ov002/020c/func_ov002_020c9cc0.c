extern signed char data_02092110;
void func_ov002_020c9cc0(char* c){
  *(short*)(c+0x6a6)=3;
  *(unsigned char*)(c+0x6e3)=6;
  *(unsigned char*)(c+0x716)=1;
  *(unsigned char*)(c+0x713)=0;
  if(*(int*)(c+0xa8)>=0x14000) *(int*)(c+0xa8)=0x14000;
  if(data_02092110==0x26 || data_02092110==0x12) *(unsigned char*)(c+0x6f5)=0;
}
