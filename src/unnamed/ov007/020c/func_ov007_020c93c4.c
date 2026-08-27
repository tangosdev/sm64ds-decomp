extern short func_ov007_020c3be0(void* a, unsigned short h1, unsigned short h2);

void func_ov007_020c93c4(void* a, char* b, char* c, short* d){
  int i = 0;
  unsigned short* pb = (unsigned short*)(b+4);
  unsigned short* pc = (unsigned short*)(c+4);
  do {
    (d + i)[2] = func_ov007_020c3be0(a, pb[i], pc[i]);
    i++;
  } while (i < 0x20);
}
