extern short data_02082214[];
void func_ov077_02123814(char* c){
  unsigned short v = *(unsigned short*)(c+0x418);
  short s = *(short*)((char*)data_02082214 + ((v>>4)<<2));
  long long prod = (long long)s * 0x27000;
  *(int*)(c+0x414) = (int)((prod + 0x800) >> 12);
  *(short*)(c+0x418) = (short)(*(short*)(c+0x418) + 0x700);
}
