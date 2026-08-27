void func_ov006_020f5694(char* c) {
  if (*(unsigned char*)(c + 0x5000 + 0x3d0) == 0) return;
  {
    unsigned short* h = (unsigned short*)(c + 0x53cc);
    *h = *h + 1;
    if (*h < 0x14) return;
    *h = 0;
  }
  {
    unsigned char* p = (unsigned char*)(c + 0x53d1);
    *p = *p + 1;
    *p = *p & 1;
  }
}
