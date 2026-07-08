extern short SINE_TABLE[];
void func_ov006_020e59b0(char*c,int i){
  char*o=c+i*0x30;
  int h=*(unsigned short*)(o+0x4686);
  int idx=(((h>>4)<<1)+1)<<1;
  int s=*(short*)((char*)SINE_TABLE+idx);
  int v=*(int*)(o+0x4668);
  long long m=(long long)s*v;
  int hi=(int)(((unsigned long long)(m+0x800))>>12);
  *(short*)(o+0x4682)=(short)((-hi)>>2);
}
