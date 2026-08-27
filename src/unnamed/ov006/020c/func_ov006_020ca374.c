struct S2{int w[2];};
extern struct S2 data_ov006_0213b124;
void func_ov006_020ca374(char *o, short v){
  *(short*)(o+0x6c)=v;
  *(int*)(o+0x48)=0;
  *(struct S2*)(o+0x70)=data_ov006_0213b124;
}
