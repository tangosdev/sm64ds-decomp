extern int data_ov006_02142f74;
extern int data_ov006_02142f7c;
void func_ov006_02122b24(void){
  int i = 0;
  if(data_ov006_02142f74 > 0){
    int off = 0;
    do {
      *(short*)(data_ov006_02142f7c + off + 0x76) = 0;
      i++;
      off += 0x78;
    } while(i < data_ov006_02142f74);
  }
}
