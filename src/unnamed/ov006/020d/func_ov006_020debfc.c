extern void func_ov006_020ded00(void* p);
void func_ov006_020debfc(char* c){
  int i;
  char* p = c;
  for (i = 0; i < 0x20; i++) {
    if (*(unsigned char*)(p+0x15)) func_ov006_020ded00(p);
    p += 0x18;
  }
}
