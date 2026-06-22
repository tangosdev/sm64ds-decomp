extern void func_ov006_0210c638(void* thiz);
extern void func_ov006_0210c354(void* p);
void func_ov006_0210c674(char* c, int i){
  if(i == 4){
    *(unsigned char*)(c+0x4706) = *(unsigned char*)(c+0x4709);
  } else if(i == 3){
    func_ov006_0210c638(c);
  }
  func_ov006_0210c354(c+0x4660);
  *(int*)(c+0x46b4) = 0;
}
