extern unsigned short data_020a0f1c;
extern unsigned char FRAME_COUNTER[];
void func_0203da4c(void){
  unsigned int v = data_020a0f1c & 0xff60;
  *(unsigned char*)(FRAME_COUNTER+0x22) = 0;
  data_020a0f1c = v;
  *(unsigned char*)(FRAME_COUNTER+0x22) = 0;
}
