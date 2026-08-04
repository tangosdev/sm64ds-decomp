extern void func_ov006_020dec88(void* a);

void func_ov006_020debb4(char* a, int b){
  int i = 0;
  do {
    if (*(unsigned char*)(a+0x15) != 0 && b == *(signed char*)(a+0x17))
      func_ov006_020dec88(a);
    i++;
    a += 0x18;
  } while (i < 0x20);
}
