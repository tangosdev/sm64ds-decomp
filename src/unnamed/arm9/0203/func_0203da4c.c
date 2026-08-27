extern unsigned short data_020a0f1c;
extern unsigned char data_020a1040[];
void func_0203da4c(void){
  unsigned int v = data_020a0f1c & 0xff60;
  *(unsigned char*)(data_020a1040+0x22) = 0;
  data_020a0f1c = v;
  *(unsigned char*)(data_020a1040+0x22) = 0;
}
