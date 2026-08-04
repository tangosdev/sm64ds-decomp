extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213e6a8;
void func_ov006_0210c234(unsigned char* o){
  if(*(unsigned char*)(o+8)==0) return;
  RenderOamMainScreen(&data_ov006_0213e6a8, *(short*)o, *(short*)(o+2), -1, -1);
}
