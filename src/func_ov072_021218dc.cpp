//cpp
typedef int Fix12i;
extern "C" int _ZN9Animation8FinishedEv(void* a);
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* ma, void* f, int b, Fix12i c, unsigned int d);
extern "C" void func_ov072_02121d50(void* self, int x);
extern "C" void _ZN9Animation7AdvanceEv(void* a);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* clsn);
extern "C" void func_ov072_02120fd4(int* self, void* clsn);
extern "C" void func_ov072_02120f14(void* self);
extern "C" void func_ov072_02120e50(void* self);
extern "C" void _ZN12CylinderClsn5ClearEv(void* c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* c);
extern void* data_ov072_02122cac[];
extern void* data_ov072_02122ca4[];

extern "C" int func_ov072_021218dc(char* self){
  unsigned char* pb;
  int* pi;
  switch(*(unsigned char*)(self+0x36e)){
  case 0:
    if(_ZN9Animation8FinishedEv(self+0x124)){
      _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self+0xd4, data_ov072_02122cac[1], 0, 0x1000, 0);
      *(int*)(self+0x130)=0x1000;
      pb=(unsigned char*)(((int)self+0x36e) & 0xFFFFFFFFFFFFFFFFLL); (*pb)++;
    }
    break;
  case 1:
    pi=(int*)(((int)self+0x98) & 0xFFFFFFFFFFFFFFFFLL);
    *pi = *pi - 0x3000;
    if(*(int*)(self+0x98) < 0){
      _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self+0xd4, data_ov072_02122ca4[1], 0x40000000, 0x1000, 0);
      *(int*)(self+0x98)=0;
      pb=(unsigned char*)(((int)self+0x36e) & 0xFFFFFFFFFFFFFFFFLL); (*pb)++;
    }
    break;
  case 2:
    if(_ZN9Animation8FinishedEv(self+0x124)){
      func_ov072_02121d50(self, 0);
    }
    break;
  }
  _ZN9Animation7AdvanceEv(self+0x124);
  _ZN5Actor9UpdatePosEP12CylinderClsn(self, self+0x160);
  func_ov072_02120fd4((int*)self, self+0x194);
  func_ov072_02120f14(self);
  func_ov072_02120e50(self);
  _ZN12CylinderClsn5ClearEv(self+0x160);
  _ZN12CylinderClsn6UpdateEv(self+0x160);
  return 1;
}
