//cpp
extern "C" {
void _ZN19CylinderClsnWithPosD1Ev(void* c);
void _ZN6Memory16operator_delete2EPv(void* p);
}
extern char* data_ov002_02110a48[];
extern "C" int _ZN4Tree16CleanupResourcesEv(char* c){
  char* r7 = c + 0xd4;
  char** r6 = data_ov002_02110a48;
  int i;
  for (i = 0; i < 5; i++){
    if (*(int*)(r7 + 0x4c) != 0){
      while (*r6 != 0){
        char* r4 = *r6;
        *r6 = *(char**)(r4 + 0x48);
        if (r4 != 0){
          _ZN19CylinderClsnWithPosD1Ev(r4 + 0xc);
          _ZN6Memory16operator_delete2EPv(r4);
        }
      }
    }
    r7 += 0x50;
    r6 += 1;
  }
  return 1;
}
