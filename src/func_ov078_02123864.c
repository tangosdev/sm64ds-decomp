extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
void func_ov078_02123864(char* r7) {
  int i = 0;
  do {
    char* a = (char*)_ZN8dActor_c10FindWithIDEj(((unsigned int*)(r7 + 0x424))[i]);
    if (a) {
      *(int*)(a + 0x3e0) = 0;
      *(unsigned char*)(a + 0x3f6) = 1;
    }
    i++;
  } while (i < 2);
}
