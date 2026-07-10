//cpp
extern "C" {
extern void DecIfAbove0_Short(void* p);
extern void func_ov071_02120278(char* c);
extern void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(char* self, void* clsn);
extern int _ZNK12WithMeshClsn14GetResultFlag1Ev(void* self);
extern int _ZNK12WithMeshClsn12TouchesWaterEv(void* self);
extern void func_ov071_0211f498(char* c);
extern void func_ov071_0211f524(char* c);
int _ZN10Scuttlebug8BehaviorEv(char* c){
  DecIfAbove0_Short(c+0x3a8);
  func_ov071_02120278(c);
  _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(c, c+0x160);
  if(_ZNK12WithMeshClsn14GetResultFlag1Ev(c+0x194) && _ZNK12WithMeshClsn12TouchesWaterEv(c+0x194)){
    func_ov071_0211f498(c);
  }
  func_ov071_0211f524(c);
  return 1;
}
}
