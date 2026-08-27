void func_ov004_020adf70(char* c, short a1, short a2, int* r3, int* sp0, int* sp4){
  *(short*)(c+0x1e)=a1;
  *(short*)(c+0x1a)=a2;
  *(short*)(c+0x1c)=*(short*)(c+0x1a);
  *(int*)(c)=r3[0];
  *(int*)(c+4)=r3[1];
  if(sp0){
    *(int*)(c+8)=sp0[0];
    *(int*)(c+0xc)=sp0[1];
  }
  if(sp4){
    *(int*)(c+0x10)=sp4[0];
    *(int*)(c+0x14)=sp4[1];
  }
}
