void func_ov075_0211ad60(char* c){
  int i;
  for(i=0;i<*(unsigned short*)(c+0xa4);i++){
    int* e = (int*)(*(int*)(c+0x80) + i*0x18);
    if(e[0]==0 || e[0]==0x1f4000 || e[1]==0 || e[1]==0x1f4000)
      e[2]=0;
  }
}
