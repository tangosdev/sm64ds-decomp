void func_02021c90(char* c){
  int* p;
  int i;
  p = (int*)(c + 8);
  do {
    *p = 0;
    p = (int*)((char*)p + 0x1c);
  } while (p != (int*)(c + 0x708));
  *(int*)c = 0;
  *(unsigned char*)(c + 4) = 0;
  for (i = 0; i < 0x10; i++)
    ((int*)(c + 0x708))[i] = 0;
}
