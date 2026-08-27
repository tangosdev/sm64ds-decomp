extern int _ZN6Player6IsAnimEj(void*, unsigned int);
extern int data_ov002_0210a5b4[];
int func_ov002_020e0a64(char* c){
  if (_ZN6Player6IsAnimEj(c, 0x54) == 0)
    *(unsigned short*)(c + 0x6a8) = 8;
  if (*(unsigned char*)(c + 0x703) != 0)
    return 0x9b;
  return data_ov002_0210a5b4[*(unsigned char*)(c + 0x6e1)];
}
