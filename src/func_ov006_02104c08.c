void func_ov006_02104c08(char* c) {
  unsigned short* h = (unsigned short*)(c + 0x4682);
  *h = *h + 1;
  if (*(unsigned short*)(c + 0x4600 + 0x82) < 4) return;
  *(unsigned short*)(c + 0x4600 + 0x82) = 0;
  {
    int* w = (int*)(c + 0x467c);
    *w = *w - 0x4000;
  }
  *(unsigned char*)(c + 0x4000 + 0x686) = 1;
}
