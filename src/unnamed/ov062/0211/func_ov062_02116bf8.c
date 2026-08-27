typedef int Fix12i; typedef short s16; typedef unsigned short u16;
extern void _Z14ApproachLinearRiii(int* x, int target, int step);
extern int ApproachAngle(s16* target, s16 from, s16 start, s16 speed, s16 max);
extern int AngleDiff(s16 a, s16 b);
extern int Chuckya_ChangeState(void* c, void* p);
extern int data_ov062_0211dee0[];
int func_ov062_02116bf8(char* c){
  _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x2000);
  if(*(u16*)(c+0x3e6) == 0){
    ApproachAngle((s16*)(c+0x94), *(s16*)(c+0x3f4), 0xa, 0x200, 0x100);
    if(AngleDiff(*(s16*)(c+0x3f4), *(s16*)(c+0x8e)) < 0x100){
      Chuckya_ChangeState(c, data_ov062_0211dee0);
    }
  }
  return 1;
}
