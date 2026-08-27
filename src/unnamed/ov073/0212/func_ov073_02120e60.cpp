//cpp
extern "C" {
extern int _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(char* self, void* bca, int a, int b, int fix, unsigned short t);
extern short Vec3_HorzAngle(void* a, void* b);
extern int data_ov073_02123288[];
int func_ov073_02120e60(char* c){
  _ZN14BlendModelAnim7SetAnimER8BCA_Fileii5Fix12IiEt(c+0x30c, (void*)data_ov073_02123288[1], 4, 0, 0x1000, 0);
  *(int*)(c+0x368) = 0x2000;
  *(short*)(c+0x4c6) = Vec3_HorzAngle(c+0x5c, c+0x3d8);
  *(int*)(c+0x4b4) = 0;
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  return 1;
}
}
