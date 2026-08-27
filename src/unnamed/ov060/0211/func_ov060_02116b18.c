extern int RandomIntInternal(void*);
extern int data_0209e650[];
void func_ov060_02116b18(char* c){
  *(int*)(c+0xa8) = 0x7000;
  *(int*)(c+0x98) = 0x11800;
  *(int*)(c+0x360) = 0x2000;
  *(int*)(c+0x9c) = 0x1000;
  *(unsigned short*)(c+0x376) = (unsigned int)RandomIntInternal(data_0209e650) >> 0x10;
  *(int*)(c+0x368) = 0x10;
}
