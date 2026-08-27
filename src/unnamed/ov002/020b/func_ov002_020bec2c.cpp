//cpp
extern "C" {
extern int data_ov002_020ff480[];
extern int _ZN9Animation8LoadFileER13SharedFilePtr(void*);
void func_ov002_020bec2c(void){
  int i;
  for(i=0x44;i<0x1c8;i+=4){
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+1]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+2]);
    _ZN9Animation8LoadFileER13SharedFilePtr((void*)data_ov002_020ff480[i+3]);
  }
}
}
