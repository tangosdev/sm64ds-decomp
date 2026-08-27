//cpp
extern "C" {
int func_ov062_02115f84(char* c);
void Chuckya_ChangeState(void*, void*);
int AngleDiff(int a, int b);
int ApproachAngle(void*, int, int, int, int);
extern int data_ov062_0211dee0[];
int func_ov062_02116894(char* c){
  if(*(unsigned short*)(c+0x100) != 0) return 1;
  if(*(int*)(c+0x98) == 0) goto angle;
  if(func_ov062_02115f84(c) != 0 || *(unsigned char*)(c+0x3e4) == 1){
    *(short*)((c+0x300)+0xf4) = *(short*)(c+0x94) - 0x2000;
    goto angle;
  }
  *(short*)(c+0x94) = *(short*)((c+0x300)+0xf4);
  Chuckya_ChangeState(c, data_ov062_0211dee0);
  *(short*)((c+0x300)+0xe8) = 0x1e;
  *(short*)((c+0x300)+0xf4) = *(short*)(c+0x94);
  return 1;
angle:
  if(AngleDiff(*(short*)((c+0x300)+0xf4), *(short*)(c+0x8e)) < 0x100){
    *(int*)(c+0x98) = 0xa000;
  }
  ApproachAngle(c+0x94, *(short*)((c+0x300)+0xf4), 0xa, 0x200, 0x100);
  return 1;
}
}
