extern char* data_020a9db8;
void func_02065f08(int idx){
  unsigned short r1;
  unsigned short r3;
  char* b = data_020a9db8 + idx * 0x5c4;
  if(*(unsigned char*)(b + 0x1d4a) == 0) return;
  if(*(unsigned short*)(b + 0x1d44) == 0) return;
  r1 = *(unsigned short*)(b + 0x1d40);
  r3 = *(unsigned short*)(b + 0x1d42);
  if(r3 <= r1 && r1 <= r3 + 2){
    unsigned short* p = (unsigned short*)(((int)b + 0x1d40));
    *p = *p + 1;
    return;
  }
  *(unsigned short*)(b + 0x1d40) = r3;
}
