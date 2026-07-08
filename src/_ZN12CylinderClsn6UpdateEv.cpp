//cpp
extern "C" {
extern void* _ZN12CylinderClsn4lastE;
void _ZN12CylinderClsn6UpdateEv(char* c){
  if(*(int*)(c+0x18) & 1) return;
  void* h = _ZN12CylinderClsn4lastE;
  *(void**)(c+0x2c) = h;
  if(_ZN12CylinderClsn4lastE) *(void**)((char*)_ZN12CylinderClsn4lastE+0x28) = c;
  _ZN12CylinderClsn4lastE = c;
}
}
