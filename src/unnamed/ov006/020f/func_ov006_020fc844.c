extern int data_ov006_0212eb24[];
void func_ov006_020fc844(unsigned char* c){
  int i;
  for(i=0;i<2;i++,c+=0x1c){
    *(unsigned char*)(c+0x4eb7)=1;
    *(unsigned char*)(c+0x4eb8)=0;
    *(unsigned char*)(c+0x4eb3)=0;
    *(unsigned char*)(c+0x4eb4)=0;
    *(unsigned char*)(c+0x4eb5)=0;
    *(short*)(c+0x4eb0)=0;
    *(int*)(c+0x4ea0)=data_ov006_0212eb24[i]<<12;
    *(int*)(c+0x4ea4)=0x68000;
    *(int*)(c+0x4ea8)=0;
    *(int*)(c+0x4eac)=0;
    *(unsigned char*)(c+0x4eb9)=0;
    *(unsigned char*)(c+0x4eba)=0;
  }
}
