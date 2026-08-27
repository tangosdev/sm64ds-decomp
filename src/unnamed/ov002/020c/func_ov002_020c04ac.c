extern int AngleDiff(short a, short b);
int func_ov002_020c04ac(char*c){
  return AngleDiff(*(short*)(c+0x69a), *(short*)(c+0x8e)) <= 0x4000;
}
