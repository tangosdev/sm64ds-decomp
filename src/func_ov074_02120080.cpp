//cpp
extern "C" {
int func_ov074_021203e4(void* c, int i);
int func_ov074_02121a20(void* c, int idx);
int _ZN9Animation8FinishedEv(void*);
int func_ov074_02121a4c(void* c, int i);
int ApproachAngle(void*, int, int, int, int);
int func_ov074_021216f4(void*);
int _ZN5Actor13DistToCPlayerEv(void*);
int _ZN5Actor18HorzAngleToCPlayerEv(void*);
int AngleDiff(int a, int b);
void func_ov074_02120080(char* c){
  if(*(unsigned char*)(c+0x602) == 0){
    func_ov074_021203e4(c, 2);
    return;
  }
  if(func_ov074_02121a20(c, 0xa) != 0){
    if(_ZN9Animation8FinishedEv(c+0x260) == 0) return;
    func_ov074_02121a4c(c, 8);
    return;
  }
  ApproachAngle(c+0x5f6, *(short*)(c+0x500+0xf8), 0xa, 0x20, 1);
  func_ov074_021216f4(c);
  if(_ZN5Actor13DistToCPlayerEv(c) >= 0x4b0000) return;
  if(AngleDiff(_ZN5Actor18HorzAngleToCPlayerEv(c), *(short*)(c+0x8e)) <= 0x4000) return;
  func_ov074_021203e4(c, 4);
}
}
