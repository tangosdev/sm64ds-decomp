//cpp
extern "C" {
int func_ov002_020c0688(char* c){
  if (*(unsigned char*)(c+0x6de) == 0) return 1;
  if (*(unsigned char*)(c+0x70e) != 0) {
    int d = *(int*)(c+0x60) - *(int*)(c+0x644);
    if (d < 0xb4000 && d >= 0) {
      *(int*)(c+0x60) = *(int*)(c+0x644);
      return 1;
    }
  } else if (*(unsigned char*)(c+0x6e4) != 0) {
    int d = *(int*)(c+0x60) - *(int*)(c+0x644);
    if (d < 0x28000) {
      *(int*)(c+0x60) = *(int*)(c+0x644);
      return 1;
    }
  }
  return 0;
}
}
