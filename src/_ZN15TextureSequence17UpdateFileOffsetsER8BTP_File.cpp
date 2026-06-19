//cpp
extern "C" void _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File(char *c){
  int i = 0;
  if (*(int*)(c+4)) *(int*)(c+4) += (int)c;
  char *p = *(char**)(c+4);
  while (i < (int)*(unsigned short*)(c+2)){
    if (*(int*)(p+4)) *(int*)(p+4) += (int)c;
    i++;
    p += 8;
  }
  if (*(int*)(c+0xc)) *(int*)(c+0xc) += (int)c;
  char *q = *(char**)(c+0xc);
  int j = 0;
  while (j < (int)*(unsigned short*)(c+8)){
    if (*(int*)(q+4)) *(int*)(q+4) += (int)c;
    j++;
    q += 8;
  }
  if (*(int*)(c+0x10)) *(int*)(c+0x10) += (int)c;
  if (*(int*)(c+0x14)) *(int*)(c+0x14) += (int)c;
  if (*(int*)(c+0x18)) *(int*)(c+0x18) += (int)c;
  if (*(int*)(c+0x20)) *(int*)(c+0x20) += (int)c;
  char *r = *(char**)(c+0x20);
  int k = 0;
  while (k < (int)*(unsigned short*)(c+0x1c)){
    if (*(int*)(r+4)) *(int*)(r+4) += (int)c;
    k++;
    r += 0xc;
  }
}
