//cpp
extern "C" {
extern int _ZN6Player9ANIM_PTRSE[];
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
void func_ov002_020bec2c(void){
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)_ZN6Player9ANIM_PTRSE[i]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)_ZN6Player9ANIM_PTRSE[i+1]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)_ZN6Player9ANIM_PTRSE[i+2]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)_ZN6Player9ANIM_PTRSE[i+3]);
  }
}
}
