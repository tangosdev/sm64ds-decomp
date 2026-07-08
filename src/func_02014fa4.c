extern void* _ZN12CylinderClsn4lastE;
void func_02014fa4(char* c){
  void* prev = *(void**)(c+0x28);
  if(prev){
    *(void**)((char*)prev+0x2c) = *(void**)(c+0x2c);
  } else {
    if(_ZN12CylinderClsn4lastE == (void*)c)
      _ZN12CylinderClsn4lastE = *(void**)(c+0x2c);
  }
  void* next = *(void**)(c+0x2c);
  if(next){
    *(void**)((char*)next+0x28) = *(void**)(c+0x28);
  }
  *(void**)(c+0x28) = 0;
  *(void**)(c+0x2c) = 0;
}
