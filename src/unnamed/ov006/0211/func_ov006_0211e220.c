#pragma opt_strength_reduction off
struct E29 { unsigned char b[0x10]; };
void func_ov006_0211e220(unsigned char* c,int param){
  struct E29* a=(struct E29*)c;
  int i;
  for(i=0;i<16;i++){
    unsigned char* s=(unsigned char*)&a[i];
    unsigned char* base = s + 0x4000;
    if(base[0x96d]==0){
      unsigned char* src = c + param*0x24;
      src += 0x4000;
      int v1 = *(int*)(src + 0x660);
      unsigned char* base2 = s + 0x4900;
      *(int*)(base + 0x960) = v1;
      int v2 = *(int*)(src + 0x664);
      int zero = 0;
      *(int*)(base + 0x964) = v2;
      *(short*)(base2 + 0x68) = zero;
      base[0x96c] = zero;
      base[0x96d] = 1;
      base[0x96e] = 1;
      return;
    }
  }
}
