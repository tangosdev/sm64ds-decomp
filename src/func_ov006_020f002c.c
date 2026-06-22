struct E { unsigned char d[0x14]; };
void func_ov006_020f002c(struct E *base, int idx){
  *(unsigned*)((unsigned char*)&base[idx] + 0x47f0) = 0x1000;
}
