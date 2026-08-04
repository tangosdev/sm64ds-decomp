extern int func_ov006_020e3388(char*);
extern int func_ov006_020e2dbc(char*);
void func_ov006_020e3210(char*c){
  func_ov006_020e3388(c);
  *(unsigned char*)(c+0x4ee7)=1;
  *(short*)(c+0x4ee0)=0;
  func_ov006_020e2dbc(c);
  *(int*)(c+0x4eac)=1;
}
