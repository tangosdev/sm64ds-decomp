extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void MultiStore16(unsigned short val, void* dst, int nbytes);
extern void func_ov006_020fe1a8(char* p);
extern void func_ov006_020fc1b4(char* base, int val);

void func_ov006_020fb97c(char* c){
  volatile unsigned short sp;
  int count = 0;
  int i = 0;
  char* p = c;
  do {
    if (*(unsigned char*)(p + 0x5bd4) != 0) {
      count++;
      break;
    }
    i++;
    p += 0xc;
  } while (i < 4);
  if (count != 0)
    return;
  *(int*)(c + 0x5c10) = 2;
  *(short*)(c + 0x5c18) = 0x40;
  {
    unsigned r = _ZN3G2S13GetBG2CharPtrEv();
    sp = 0;
    MultiStore16(sp, (void*)r, 0x6000);
  }
  *(unsigned char*)(c + 0x5c30) = 0;
  func_ov006_020fe1a8(c);
  func_ov006_020fc1b4(c, 1);
}
