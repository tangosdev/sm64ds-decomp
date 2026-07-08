extern unsigned char BOTTOM_SCREEN_RELATED;
void func_ov006_0210c180(char *c){
  *(int*)c = 0x48;
  *(unsigned char*)(c+4) = 1;
  BOTTOM_SCREEN_RELATED |= 2;
}
