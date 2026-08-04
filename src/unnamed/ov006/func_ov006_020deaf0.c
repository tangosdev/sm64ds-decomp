extern void func_ov006_020dec5c(void* p, int a, int b);
void func_ov006_020deaf0(char* p, int key, int a, int b){
  int i;
  for(i=0; i<0x20; i++){
    if(*(unsigned char*)(p+0x15) != 0){
      if(key == *(signed char*)(p+0x17)){
        func_ov006_020dec5c(p, a, b);
      }
    }
    p += 0x18;
  }
}
